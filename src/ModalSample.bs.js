// Generated by ReScript, PLEASE EDIT WITH CARE
'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Modal$ReactTemplate = require("./Modal.bs.js");
var ModalBody$ReactTemplate = require("./ModalBody.bs.js");
var ModalFooter$ReactTemplate = require("./ModalFooter.bs.js");
var ModalHeader$ReactTemplate = require("./ModalHeader.bs.js");

function ModalSample(Props) {
  var match = React.useReducer((function (param, action) {
          if (action) {
            return {
                    isOpen: false
                  };
          } else {
            return {
                    isOpen: true
                  };
          }
        }), {
        isOpen: false
      });
  var dispatch = match[1];
  return React.createElement("div", undefined, React.createElement(Modal$ReactTemplate.make, {
                  hidden: !match[0].isOpen,
                  size: /* Large */0,
                  onClickContentOutside: (function (param) {
                      return Curry._1(dispatch, /* Close */1);
                    }),
                  header: React.createElement(ModalHeader$ReactTemplate.make, {
                        title: React.createElement("h3", undefined, "MODAL!"),
                        children: React.createElement("button", {
                              className: "close",
                              type: "button"
                            }, React.createElement("span", undefined, "\xc3\x97")),
                        key: "header"
                      }),
                  footer: React.createElement(ModalFooter$ReactTemplate.make, {
                        children: React.createElement("button", {
                              className: "btn btn-primary",
                              type: "button",
                              onClick: (function (param) {
                                  return Curry._1(dispatch, /* Close */1);
                                })
                            }, "CLOSE")
                      }),
                  body: React.createElement(ModalBody$ReactTemplate.make, {
                        children: React.createElement("div", undefined, React.createElement("p", {
                                  onClick: (function (param) {
                                      console.log("JS");
                                      
                                    })
                                }, "LINE1LINE1LINE1LINE1LINE1LINE1LINE1LINE1LINE1sLINE1LINE1LINE1LINE1LINE1LINE1LINE1LINE1LINE1s"), React.createElement("span", undefined, "LINE2")),
                        key: "body"
                      })
                }), React.createElement("button", {
                  onClick: (function (param) {
                      return Curry._1(dispatch, /* Open */0);
                    })
                }, "OPEN"));
}

var make = ModalSample;

exports.make = make;
/* react Not a pure module */