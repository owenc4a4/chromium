// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

onload = function() {
  var getURL = chrome.extension.getURL;
  var URL_TEST = "http://127.0.0.1:PORT/test";
  chrome.tabs.create({"url": "about:blank"}, function(tab) {
    var tabId = tab.id;
    chrome.test.getConfig(function(config) {
      var fixPort = function(url) {
        return url.replace(/PORT/g, config.testServer.port);
      };
      URL_TEST = fixPort(URL_TEST);

      chrome.test.runTests([
        // Navigates to a different site, but then modifies the reference
        // framgent.
        function crossProcessFragment() {
          expect([
            { label: "a-onBeforeNavigate",
              event: "onBeforeNavigate",
              details: { frameId: 0,
                         parentFrameId: -1,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: getURL('crossProcess/f.html') }},
            { label: "a-onCommitted",
              event: "onCommitted",
              details: { frameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         transitionQualifiers: [],
                         transitionType: "link",
                         url: getURL('crossProcess/f.html') }},
            { label: "a-onDOMContentLoaded",
              event: "onDOMContentLoaded",
              details: { frameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: getURL('crossProcess/f.html') }},
            { label: "a-onCompleted",
              event: "onCompleted",
              details: { frameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: getURL('crossProcess/f.html') }},
            { label: "a-onReferenceFragmentUpdated",
              event: "onReferenceFragmentUpdated",
              details: { frameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         transitionQualifiers: [],
                         transitionType: "link",
                         url: getURL('crossProcess/f.html#foo') }},
            { label: "b-onBeforeNavigate",
              event: "onBeforeNavigate",
              details: { frameId: 0,
                         parentFrameId: -1,
                         processId: 1,
                         tabId: 0,
                         timeStamp: 0,
                         url: URL_TEST + "3" }},
            { label: "b-onErrorOccurred",
              event: "onErrorOccurred",
              details: { error: "net::ERR_ABORTED",
                         frameId: 0,
                         processId: 1,
                         tabId: 0,
                         timeStamp: 0,
                         url: URL_TEST + "3" }}],
            [ navigationOrder("a-"),
              [ "a-onCompleted", "b-onBeforeNavigate",
                "a-onReferenceFragmentUpdated"] ]);

          chrome.tabs.update(
              tabId,
              { url: getURL('crossProcess/f.html?' + config.testServer.port) });
        },

        // A page with an iframe that changes its reference fragment before
        // the iframe committed.
        function crossProcessFragmentIFrame() {
          expect([
            { label: "a-onBeforeNavigate",
              event: "onBeforeNavigate",
              details: { frameId: 0,
                         parentFrameId: -1,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: getURL('crossProcess/g.html') }},
            { label: "a-onCommitted",
              event: "onCommitted",
              details: { frameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         transitionQualifiers: [],
                         transitionType: "link",
                         url: getURL('crossProcess/g.html') }},
            { label: "a-onDOMContentLoaded",
              event: "onDOMContentLoaded",
              details: { frameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: getURL('crossProcess/g.html') }},
            { label: "a-onCompleted",
              event: "onCompleted",
              details: { frameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: getURL('crossProcess/g.html') }},
            { label: "a-onReferenceFragmentUpdated",
              event: "onReferenceFragmentUpdated",
              details: { frameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         transitionQualifiers: [],
                         transitionType: "link",
                         url: getURL('crossProcess/g.html#foo') }},
            { label: "b-onBeforeNavigate",
              event: "onBeforeNavigate",
              details: { frameId: 1,
                         parentFrameId: 0,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: URL_TEST + "4" }},
            { label: "b-onCommitted",
              event: "onCommitted",
              details: { frameId: 1,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         transitionQualifiers: [],
                         transitionType: "auto_subframe",
                         url: URL_TEST + "4" }},
            { label: "b-onDOMContentLoaded",
              event: "onDOMContentLoaded",
              details: { frameId: 1,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: URL_TEST + "4" }},
            { label: "b-onCompleted",
              event: "onCompleted",
              details: { frameId: 1,
                         processId: 0,
                         tabId: 0,
                         timeStamp: 0,
                         url: URL_TEST + "4" }}],
            [ navigationOrder("a-"), navigationOrder("b-"),
              [ "a-onCompleted", "b-onBeforeNavigate",
                "a-onReferenceFragmentUpdated"] ]);

          chrome.tabs.update(
              tabId,
              { url: getURL('crossProcess/g.html?' + config.testServer.port) });
        },

      ]);
    });
  });
};
