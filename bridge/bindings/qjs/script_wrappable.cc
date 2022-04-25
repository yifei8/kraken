/*
 * Copyright (C) 2019 Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

#include "script_wrappable.h"
#include "core/executing_context.h"
#include "cppgc/gc_visitor.h"

namespace kraken {

ScriptWrappable::ScriptWrappable(JSContext* ctx) : ctx_(ctx), runtime_(JS_GetRuntime(ctx)) {}

JSValue ScriptWrappable::ToQuickJS() {
  return JS_DupValue(ctx_, jsObject_);
}

JSValue ScriptWrappable::ToQuickJSUnsafe() const {
  return jsObject_;
}

ScriptValue ScriptWrappable::ToValue() {
  return ScriptValue(ctx_, jsObject_);
}

/// This callback will be called when QuickJS GC is running at marking stage.
/// Users of this class should override `void Trace(JSRuntime* rt, JSValueConst val, JS_MarkFunc* mark_func)` to
/// tell GC which member of their class should be collected by GC.
static void HandleJSObjectGCMark(JSRuntime* rt, JSValueConst val, JS_MarkFunc* mark_func) {
  auto* object = static_cast<ScriptWrappable*>(JS_GetOpaque(val, JSValueGetClassId(val)));
  GCVisitor visitor{rt, mark_func};
  object->Trace(&visitor);
}

/// This callback will be called when QuickJS GC will release the `jsObject` object memory of this class.
/// The deconstruct method of this class will be called and all memory about this class will be freed when finalize
/// completed.
static void HandleJSObjectFinalized(JSRuntime* rt, JSValue val) {
  auto* object = static_cast<ScriptWrappable*>(JS_GetOpaque(val, JSValueGetClassId(val)));
  delete object;
}

/// This callback will be called when JS code access this object using [] or `.` operator.
/// When exec `obj[1]`, it will call indexed_property_getter_handler_ defined in WrapperTypeInfo.
/// When exec `obj['hello']`, it will call string_property_getter_handler_ defined in WrapperTypeInfo.
static JSValue HandleJSPropertyGetterCallback(JSContext* ctx, JSValueConst obj, JSAtom atom, JSValueConst receiver) {
  auto* object = static_cast<ScriptWrappable*>(JS_GetOpaque(obj, JSValueGetClassId(obj)));
  auto* wrapper_type_info = object->GetWrapperTypeInfo();

  if (wrapper_type_info->indexed_property_getter_handler_ != nullptr && JS_AtomIsTaggedInt(atom)) {
    return wrapper_type_info->indexed_property_getter_handler_(ctx, obj, JS_AtomToUInt32(atom));
  }
  return wrapper_type_info->string_property_getter_handler_(ctx, obj, atom);
}

/// This callback will be callback when JS code set property on this object using [] or `.` operator.
/// When exec `obj[1] = 1`, it will call
static int HandleJSPropertySetterCallback(JSContext* ctx,
                                          JSValueConst obj,
                                          JSAtom atom,
                                          JSValueConst value,
                                          JSValueConst receiver,
                                          int flags) {
  auto* object = static_cast<ScriptWrappable*>(JS_GetOpaque(obj, JSValueGetClassId(obj)));
  auto* wrapper_type_info = object->GetWrapperTypeInfo();

  if (wrapper_type_info->indexed_property_setter_handler_ != nullptr && JS_AtomIsTaggedInt(atom)) {
    return wrapper_type_info->indexed_property_setter_handler_(ctx, obj, JS_AtomToUInt32(atom), value);
  }

  return wrapper_type_info->string_property_setter_handler_(ctx, obj, atom, value);
}

void ScriptWrappable::InitializeQuickJSObject() {
  auto* wrapper_type_info = GetWrapperTypeInfo();
  JSRuntime* runtime = runtime_;

  /// ClassId should be a static QJSValue to make sure JSClassDef when this class are created at the first class.
  if (!JS_HasClassId(runtime, wrapper_type_info->classId)) {
    /// Basic template to describe the behavior about this class.
    JSClassDef def{};

    // Define object's className
    def.class_name = wrapper_type_info->className;

    // Register the hooks when GC marking at this object.
    def.gc_mark = HandleJSObjectGCMark;

    // Define the custom behavior of object.
    auto* exotic_methods = new JSClassExoticMethods{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

    // Define the callback when access object property.
    if (UNLIKELY(wrapper_type_info->indexed_property_getter_handler_ != nullptr ||
                 wrapper_type_info->string_property_getter_handler_ != nullptr)) {
      exotic_methods->get_property = HandleJSPropertyGetterCallback;
    }

    // Define the callback when set object property.
    if (UNLIKELY(wrapper_type_info->indexed_property_getter_handler_ != nullptr ||
                 wrapper_type_info->string_property_setter_handler_ != nullptr)) {
      exotic_methods->set_property = HandleJSPropertySetterCallback;
    }

    def.exotic = exotic_methods;
    def.finalizer = HandleJSObjectFinalized;

    JS_NewClass(runtime, wrapper_type_info->classId, &def);
  }

  /// The JavaScript object underline this class. This `jsObject` is the JavaScript object which can be directly access
  /// within JavaScript code. When the reference count of `jsObject` decrease to 0, QuickJS will trigger `finalizer`
  /// callback and free `jsObject` memory. When QuickJS GC found `jsObject` at marking stage, `gc_mark` callback will be
  /// triggered.
  jsObject_ = JS_NewObjectClass(ctx_, wrapper_type_info->classId);
  JS_SetOpaque(jsObject_, this);

  // Let our instance into inherit prototype methods.
  JSValue prototype = GetExecutingContext()->contextData()->prototypeForType(wrapper_type_info);
  JS_SetPrototype(ctx_, jsObject_, prototype);

  wrapped_ = true;
}

}  // namespace kraken
