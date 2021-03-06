// Generated by ReScript, PLEASE EDIT WITH CARE
'use strict';

var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");

function join(strings) {
  return List.fold_left((function (a, b) {
                return a + b;
              }), "", strings);
}

var MyString = {
  join: join
};

function Tree(L) {
  var leaf = function (t) {
    return {
            TAG: 0,
            _0: t,
            [Symbol.for("name")]: "Leaf"
          };
  };
  var node = function (tree) {
    return {
            TAG: 1,
            _0: tree,
            [Symbol.for("name")]: "Node"
          };
  };
  var nodeIdentity = function (tree) {
    if (tree.TAG === /* Leaf */0) {
      return "leaf-" + Curry._1(L.identity, tree._0);
    } else {
      return "node-" + join(List.map(nodeIdentity, tree._0));
    }
  };
  var updateLeaf = function (t, tree) {
    if (tree.TAG !== /* Leaf */0) {
      return {
              TAG: 1,
              _0: List.map((function (param) {
                      return updateLeaf(t, param);
                    }), tree._0),
              [Symbol.for("name")]: "Node"
            };
    }
    var data = tree._0;
    if (Curry._1(L.identity, data) === Curry._1(L.identity, t)) {
      return {
              TAG: 0,
              _0: t,
              [Symbol.for("name")]: "Leaf"
            };
    } else {
      return {
              TAG: 0,
              _0: data,
              [Symbol.for("name")]: "Leaf"
            };
    }
  };
  var showTree = function (tree) {
    if (tree.TAG === /* Leaf */0) {
      var data = tree._0;
      return React.createElement("span", {
                  key: nodeIdentity({
                        TAG: 0,
                        _0: data,
                        [Symbol.for("name")]: "Leaf"
                      })
                }, Curry._1(L.showLeaf, data));
    }
    var nodes = tree._0;
    var reactElmList = List.map(showTree, nodes);
    return React.createElement("div", {
                key: nodeIdentity({
                      TAG: 1,
                      _0: nodes,
                      [Symbol.for("name")]: "Node"
                    })
              }, $$Array.of_list(reactElmList));
  };
  return {
          leaf: leaf,
          node: node,
          nodeIdentity: nodeIdentity,
          updateLeaf: updateLeaf,
          showTree: showTree
        };
}

exports.MyString = MyString;
exports.Tree = Tree;
/* react Not a pure module */
