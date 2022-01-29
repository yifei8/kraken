/*
 * Copyright (C) 2021 Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

#include <atomic>

#include "foundation/logging.h"
#include "polyfill.h"
#include "bindings/qjs/qjs_patch.h"
#include "page.h"

//#include "bindings/qjs/bom/blob.h"
//#include "bindings/qjs/bom/console.h"
//#include "bindings/qjs/bom/location.h"
//#include "bindings/qjs/bom/performance.h"
//#include "bindings/qjs/bom/screen.h"
//#include "bindings/qjs/bom/timer.h"
//#include "bindings/qjs/bom/window.h"
//#include "bindings/qjs/dom/comment_node.h"
//#include "bindings/qjs/dom/custom_event.h"
//#include "bindings/qjs/dom/document.h"
//#include "bindings/qjs/dom/document_fragment.h"
//#include "bindings/qjs/dom/element.h"
//#include "bindings/qjs/dom/elements/.gen/anchor_element.h"
//#include "bindings/qjs/dom/elements/.gen/canvas_element.h"
//#include "bindings/qjs/dom/elements/.gen/input_element.h"
//#include "bindings/qjs/dom/elements/.gen/object_element.h"
//#include "bindings/qjs/dom/elements/.gen/script_element.h"
//#include "bindings/qjs/dom/elements/image_element.h"
//#include "bindings/qjs/dom/elements/template_element.h"
//#include "bindings/qjs/dom/event.h"
//#include "bindings/qjs/dom/event_target.h"
//#include "bindings/qjs/dom/events/.gen/close_event.h"
//#include "bindings/qjs/dom/events/.gen/gesture_event.h"
//#include "bindings/qjs/dom/events/.gen/input_event.h"
//#include "bindings/qjs/dom/events/.gen/intersection_change.h"
//#include "bindings/qjs/dom/events/.gen/media_error_event.h"
//#include "bindings/qjs/dom/events/.gen/message_event.h"
//#include "bindings/qjs/dom/events/.gen/mouse_event.h"
//#include "bindings/qjs/dom/events/.gen/popstate_event.h"
//#include "bindings/qjs/dom/events/touch_event.h"
//#include "bindings/qjs/dom/style_declaration.h"
//#include "bindings/qjs/dom/text_node.h"
//#include "bindings/qjs/module_manager.h"

namespace kraken {

using namespace binding::qjs;

std::unordered_map<std::string, NativeByteCode> KrakenPage::pluginByteCode{};
ConsoleMessageHandler KrakenPage::consoleMessageHandler{nullptr};

kraken::KrakenPage** KrakenPage::pageContextPool{nullptr};

KrakenPage::KrakenPage(int32_t contextId, const JSExceptionHandler& handler) : contextId(contextId), ownerThreadId(std::this_thread::get_id()) {
#if ENABLE_PROFILE
  auto jsContextStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#endif
  m_context = new ExecutionContext(contextId, [this](int32_t contextId, const char* message) {
    if (m_context->dartMethodPtr()->onJsError != nullptr) {
      m_context->dartMethodPtr()->onJsError(contextId, message);
    }
    KRAKEN_LOG(ERROR) << message << std::endl;
  }, this);

#if ENABLE_PROFILE
  auto nativePerformance = Performance::instance(m_context)->m_nativePerformance;
  nativePerformance.mark(PERF_JS_CONTEXT_INIT_START, jsContextStartTime);
  nativePerformance.mark(PERF_JS_CONTEXT_INIT_END);
  nativePerformance.mark(PERF_JS_NATIVE_METHOD_INIT_START);
#endif

//  bindConsole(m_context);
//  bindTimer(m_context);
//  bindScreen(m_context);
//  bindModuleManager(m_context);
//  bindEventTarget(m_context);
//  bindBlob(m_context);
//  bindLocation(m_context);
//  bindWindow(m_context);
//  bindEvent(m_context);
//  bindCustomEvent(m_context);
//  bindNode(m_context);
//  bindDocumentFragment(m_context);
//  bindTextNode(m_context);
//  bindCommentNode(m_context);
//  bindElement(m_context);
  //  bindAnchorElement(m_context);
  //  bindCanvasElement(m_context);
  //  bindImageElement(m_context);
  //  bindInputElement(m_context);
  //  bindObjectElement(m_context);
  //  bindScriptElement(m_context);
  //  bindTemplateElement(m_context);
//  bindCSSStyleDeclaration(m_context);
  //  bindCloseEvent(m_context);
  //  bindGestureEvent(m_context);
  //  bindInputEvent(m_context);
  //  bindIntersectionChangeEvent(m_context);
  //  bindMediaErrorEvent(m_context);
  //  bindMouseEvent(m_context);
  //  bindMessageEvent(m_context);
  //  bindPopStateEvent(m_context);
  //  bindTouchEvent(m_context);
//  bindDocument(m_context);
  //  bindPerformance(m_context);

#if ENABLE_PROFILE
  nativePerformance.mark(PERF_JS_NATIVE_METHOD_INIT_END);
  nativePerformance.mark(PERF_JS_POLYFILL_INIT_START);
#endif

  initKrakenPolyFill(this);

  for (auto& p : pluginByteCode) {
    evaluateByteCode(p.second.bytes, p.second.length);
  }

#if ENABLE_PROFILE
  nativePerformance.mark(PERF_JS_POLYFILL_INIT_END);
#endif
}

bool KrakenPage::parseHTML(const char* code, size_t length) {
//  if (!m_context->isValid())
//    return false;
//  JSValue bodyValue = JS_GetPropertyStr(m_context->ctx(), m_context->document()->jsObject, "body");
//  auto* body = static_cast<Element*>(JS_GetOpaque(bodyValue, Element::classId));
//  HTMLParser::parseHTML(code, length, body);
//  JS_FreeValue(m_context->ctx(), bodyValue);
//  return true;
}

void KrakenPage::invokeModuleEvent(const NativeString* moduleName, const char* eventType, void* ptr, NativeString* extra) {
//  if (!m_context->isValid())
//    return;
//
//  JSValue eventObject = JS_NULL;
//  if (ptr != nullptr) {
//    std::string type = std::string(eventType);
//    auto* rawEvent = static_cast<RawEvent*>(ptr)->bytes;
//    Event* event = Event::create(m_context->ctx(), reinterpret_cast<NativeEvent*>(rawEvent));
//    eventObject = event->toQuickJS();
//  }
//
//  JSValue moduleNameValue = JS_NewUnicodeString(m_context->runtime(), m_context->ctx(), moduleName->string, moduleName->length);
//  JSValue extraObject = JS_NULL;
//  if (extra != nullptr) {
//    std::u16string u16Extra = std::u16string(reinterpret_cast<const char16_t*>(extra->string), extra->length);
//    std::string extraString = toUTF8(u16Extra);
//    extraObject = JS_ParseJSON(m_context->ctx(), extraString.c_str(), extraString.size(), "");
//  }
//
//  {
//    struct list_head *el, *el1;
//    list_for_each_safe(el, el1, &m_context->module_job_list) {
//      auto* module = list_entry(el, ModuleContext, link);
//      JSValue callback = module->callback;
//
//      JSValue arguments[] = {moduleNameValue, eventObject, extraObject};
//      JSValue returnValue = JS_Call(m_context->ctx(), callback, m_context->global(), 3, arguments);
//      m_context->handleException(&returnValue);
//      JS_FreeValue(m_context->ctx(), returnValue);
//    }
//  }
//
//  JS_FreeValue(m_context->ctx(), moduleNameValue);
//
//  if (rawEvent != nullptr) {
//    JS_FreeValue(m_context->ctx(), eventObject);
//  }
//  if (extra != nullptr) {
//    JS_FreeValue(m_context->ctx(), extraObject);
//  }
}

void KrakenPage::evaluateScript(const NativeString* script, const char* url, int startLine) {
  if (!m_context->isValid())
    return;

#if ENABLE_PROFILE
  auto nativePerformance = Performance::instance(m_context)->m_nativePerformance;
  nativePerformance.mark(PERF_JS_PARSE_TIME_START);
  std::u16string patchedCode = std::u16string(u"performance.mark('js_parse_time_end');") + std::u16string(reinterpret_cast<const char16_t*>(script->string), script->length);
  m_context->evaluateJavaScript(patchedCode.c_str(), patchedCode.size(), url, startLine);
#else
  m_context->evaluateJavaScript(script->string, script->length, url, startLine);
#endif
}

void KrakenPage::evaluateScript(const uint16_t* script, size_t length, const char* url, int startLine) {
  if (!m_context->isValid())
    return;
  m_context->evaluateJavaScript(script, length, url, startLine);
}

void KrakenPage::evaluateScript(const char* script, size_t length, const char* url, int startLine) {
  if (!m_context->isValid())
    return;
  m_context->evaluateJavaScript(script, length, url, startLine);
}

uint8_t* KrakenPage::dumpByteCode(const char* script, size_t length, const char* url, size_t* byteLength) {
  if (!m_context->isValid())
    return nullptr;
  return m_context->dumpByteCode(script, length, url, byteLength);
}

void KrakenPage::evaluateByteCode(uint8_t* bytes, size_t byteLength) {
  if (!m_context->isValid())
    return;
  m_context->evaluateByteCode(bytes, byteLength);
}

void KrakenPage::registerDartMethods(uint64_t* methodBytes, int32_t length) {
  size_t i = 0;

  auto& dartMethodPointer = m_context->dartMethodPtr();

  dartMethodPointer->invokeModule = reinterpret_cast<InvokeModule>(methodBytes[i++]);
  dartMethodPointer->requestBatchUpdate = reinterpret_cast<RequestBatchUpdate>(methodBytes[i++]);
  dartMethodPointer->reloadApp = reinterpret_cast<ReloadApp>(methodBytes[i++]);
  dartMethodPointer->setTimeout = reinterpret_cast<SetTimeout>(methodBytes[i++]);
  dartMethodPointer->setInterval = reinterpret_cast<SetInterval>(methodBytes[i++]);
  dartMethodPointer->clearTimeout = reinterpret_cast<ClearTimeout>(methodBytes[i++]);
  dartMethodPointer->requestAnimationFrame = reinterpret_cast<RequestAnimationFrame>(methodBytes[i++]);
  dartMethodPointer->cancelAnimationFrame = reinterpret_cast<CancelAnimationFrame>(methodBytes[i++]);
  dartMethodPointer->getScreen = reinterpret_cast<GetScreen>(methodBytes[i++]);
  dartMethodPointer->devicePixelRatio = reinterpret_cast<DevicePixelRatio>(methodBytes[i++]);
  dartMethodPointer->platformBrightness = reinterpret_cast<PlatformBrightness>(methodBytes[i++]);
  dartMethodPointer->toBlob = reinterpret_cast<ToBlob>(methodBytes[i++]);
  dartMethodPointer->flushUICommand = reinterpret_cast<FlushUICommand>(methodBytes[i++]);
  dartMethodPointer->initWindow = reinterpret_cast<InitWindow>(methodBytes[i++]);
  dartMethodPointer->initDocument = reinterpret_cast<InitDocument>(methodBytes[i++]);

#if ENABLE_PROFILE
  methodPointer->getPerformanceEntries = reinterpret_cast<GetPerformanceEntries>(methodBytes[i++]);
#else
  i++;
#endif

  dartMethodPointer->onJsError = reinterpret_cast<OnJSError>(methodBytes[i++]);

  assert_m(i == length, "Dart native methods count is not equal with C++ side method registrations.");
}

KrakenPage::~KrakenPage() {
#if IS_TEST
  if (disposeCallback != nullptr) {
    disposeCallback(this);
  }
#endif
  delete m_context;
  KrakenPage::pageContextPool[contextId] = nullptr;
}

void KrakenPage::reportError(const char* errmsg) {
  m_handler(m_context->getContextId(), errmsg);
}

}  // namespace kraken
