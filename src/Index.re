include PromiseWrapper;

let andThen = (a, b, v) => b(a(v));
let (>>) = andThen;
let toFragment = fn => (. a) => fn(a);
let (~>) = toFragment;

[@bs.module "./externalJsModules/ExecuteSomeService"]
external execute: int => int = "execute";
let executeResult = execute(100);

Js.Console.log(executeResult);

[@bs.module "./externalJsModules/ExecuteSomeService"]
external executeNullable: int => Js.Undefined.t(int) = "executeNullable";
let executeNullableResult = executeNullable(100);
Js.Console.log(executeNullableResult);
let _ = {
  open Js;
  // open Js.Promise;
  let executeNullableResultOpt: option(int) =
    Js.Undefined.toOption(executeNullableResult);
  let appendOne = (+)(1);
  let r =
    (
      executeNullableResultOpt
      |> Option.map((. a) => appendOne(a))
      |> Option.map(~>appendOne)
      |> Belt.Option.map(_, appendOne)
    )
    ->(Belt.Option.map(appendOne));
  Console.log(r);
};

[@bs.module "./externalJsModules/ExecuteSomeService"]
external executePromise: int => Js.Promise.t(int) = "executePromise";
let executePromiseResult = executePromise(100);

Js.Promise.(
  executePromiseResult
  |> then_((+)(100) >> resolve)
  |> then_(andThen(Js.Console.log, resolve))
);

type argObj = {. name: string};

let sampleArgObj: argObj = {pub name = "hoge"};

[@bs.deriving abstract]
type argJsObj = {name: string};

[@bs.module "./externalJsModules/ExecuteSomeService"]
external executeArgObj: argJsObj => argJsObj = "executeArgObj";
let _ = {
  let argJsObjSample = argJsObj(~name="RYO");
  Js.Console.log(executeArgObj(argJsObjSample));
  Js.Console.log(argJsObjSample->nameGet);
  Js.Console.log({js|りょうた|js});
  Js.Console.log({j|りょうた|j});
};

module IntPromiseWrapperDef = {
  type v = int;
};

module IntPromiseWrapper = PromiseWrapper(IntPromiseWrapperDef);

let whenSuccess = v => <div> {React.string(string_of_int(v))} </div>;

let whenError = _ => <div> <p> {React.string("ERROR!")} </p> </div>;

let whenPending = <div> <p> {React.string("NowLoading...")} </p> </div>;

let r = () => ();

let timePromise = count =>
  Js.Promise.make((~resolve, ~reject as _) => {
    /* https://bucklescript.github.io/bucklescript/Manual.html#__code_bs_code_for_explicit_uncurried_callback */
    /* (. 引数) で引数をuncurryすることを明示してる */
    /* promiseのここだとよくわからないけど
          Array.mapとか
          Array.map((n) => 1 + n, [4,4])みたいにbsで書くと
          Array.map((n) => 1 + n)([4,4])ってjsになっちゃうからかな？
       */
    let _ = Js.Global.setTimeout(() => resolve(. count), count);
    ();
  });

module TextIncrementalSearchDef = {
  open Helpers;
  type entity = string;
  let allEntities = ["Apple", "Banana", "All", "Berry"];
  let findEntities = text =>
    Js.Promise.make((~resolve, ~reject as _) =>
      resolve(. allEntities |> List.filter(String.contain(_, text)))
    );
};

module TextIncrementalSearch =
  IncrementalSearch.IncrementalSearch(TextIncrementalSearchDef);

module type SamplePageDependentsResolver = {
  let loadResource: int => Js.Promise.t(int);
};

module SamplePage = (Resolver: SamplePageDependentsResolver) => {
  type resource = (int, string);
  type loadResourceArg = string;
  type t = {
    value: int,
    name: string,
  };
  let componentName = "SampleIntPage";
  let initialize = resource => {
    value: resource |> fst,
    name: resource |> snd,
  };

  let render = t => {
    let name = t.name;
    let valueStr = t.value |> string_of_int;
    React.string(name ++ valueStr);
  };

  let loadResource = arg =>
    Js.Promise.(
      Resolver.loadResource(1009)
      |> then_(value => (value, string_of_int(value) ++ arg) |> resolve)
    );
};

module SamplePageResolverImpl: SamplePageDependentsResolver = {
  let loadResource = arg => timePromise(arg);
};
module ResourcePromiseWrapperDef = {
  type v = (int, string);
};

module SamplePageImpl =
  Page.Make((SamplePage(SamplePageResolverImpl)), ResourcePromiseWrapperDef);

let add = (. x, y, z) => x + y + z;
let addFiveOops = add(. 5, 4, 4);

let add2 = (. x, y, z) => x + y + z;
let addFiveOops2 = add2(. 5, 4, 4);

let add3 = (x, y, z) => x + y + z;
let addFiveOops3 = add3(5, 4, 4);

type sampleRoute =
  | Home
  | About
  | NotFound;

module SampleRouting: Routing.Routing = {
  type route = sampleRoute;
  let urlToRoute = (url: ReasonReact.Router.url, _) =>
    switch (url.path) {
    | ["src", "index.html"] => Home
    | ["about"] => About
    | route =>
      Js.Console.log(route);
      NotFound;
    };
  let transition = route =>
    switch (route) {
    | Home =>
      timePromise(3000)
      |> Js.Promise.then_(v =>
           <div> {React.string(string_of_int(v))} </div>
           |> Js.Promise.resolve
         )
    | About => <div> {React.string("About")} </div> |> Js.Promise.resolve
    | NotFound => <div> {React.string("NF")} </div> |> Js.Promise.resolve
    };
};

module SampleApp = Routing.Application(SampleRouting);

type user = {
  name: string,
  id: string,
};

let userFactory = name => {name, id: name};

module UserLeaf = {
  type t = user;
  let identity = user => user.id;
  let showLeaf = user => <p key={user.id}> {React.string(user.name)} </p>;
};

module UserTree = TreeData.Tree(UserLeaf);

let userTreeSample = {
  let leaf1 = UserTree.leaf(userFactory("user1"));
  let leaf2 = UserTree.leaf(userFactory("user2"));
  let leaf3 = UserTree.leaf(userFactory("user3"));
  let leaf4 = UserTree.leaf(userFactory("user4"));
  let leaf5 = UserTree.leaf(userFactory("user5"));

  let node1Tmp = UserTree.node([leaf1, leaf2]);
  let node2 = UserTree.node([leaf4]);

  let node1 = UserTree.node([node1Tmp, leaf5]);

  let tree = UserTree.node([node1, node2, leaf3]);
  tree;
};

/* Js.Console.log(userTreeSample); */

let _ = UrlParser.Sample.start();

ReactDOMRe.renderToElementWithId(
  <div>
    <Tooltip
      position=Tooltip.Down
      size=Tooltip.Large
      text="IAM TOOLTIP"
      parent={<button className="target"> {React.string("HELLO")} </button>}
    />
    <TodoFlexibleTable.TodoTableSample />
    <IntPromiseWrapper
      promise={timePromise(1000)}
      whenError
      whenPending
      whenSuccess
    />
    <ModalSample />
    <TextIncrementalSearch
      searchResultView={results =>
        <ul>
          {results
           |> List.map(text => <li key=text> {text |> React.string} </li>)
           |> Array.of_list
           |> ReasonReact.array}
        </ul>
      }
    />
    <SamplePageImpl loadResourceArg="HOGE" />
    <SampleApp
      initialPage={<div> {React.string("INITIAL")} </div>}
      onError={_ => Js.Console.error("ERROR!")}
      onStartTransition={() => Js.Console.log("start_transition")}
      onFinishTransition={() => Js.Console.log("finish_transition")}
    />
    {UserTree.showTree(userTreeSample)}
  </div>,
  "index",
);

let plus = (a: int, b: int): int => a + b;
