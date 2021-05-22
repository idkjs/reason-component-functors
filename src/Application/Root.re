module Utils = {
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
};

open Operator;

let (==>) = (a, b) => (a, b);

type appRoute =
  | Home(int)
  | About(string)
  | NotFound;

module type GlobalStore = {
  type action;
  type state;
  let store: GlobalStateManagement.Manager.t(action, state);

  let renderWithStore:
    (
      ~render: GlobalStateManagement.Manager.t(action, state) => React.element
    ) =>
    React.element;
};

module type HomeExternalService = {let fetchDataHome: string => string;};

module type AboutExternalService = {let fetchDataAbout: string => string;};

module HomeExternalServiceImpl: HomeExternalService = {
  let fetchDataHome = str => str ++ "!";
};

module AboutExternalServiceImpl: AboutExternalService = {
  let fetchDataAbout = str => str ++ "!";
};

module type AllExternalSercice = {
  include HomeExternalService;
  include AboutExternalService;
};

module AllExternalSerciceImpl: AllExternalSercice = {
  include HomeExternalServiceImpl;
  include AboutExternalServiceImpl;
};

module StateManagementDef = {
  type error = {messages: list(string)};
  type notification = {messages: list(string)};
  let getMessages = ({messages}: notification) => messages;
  type action =
    | DetectedError(error)
    | RefreshError;

  type state = {
    error: option(error),
    notification: option(notification),
  };
};

module MakeHomeContainer =
       (
         Store: GlobalStore with type state = StateManagementDef.state,
         HomeExternalService: HomeExternalService,
       ) => {
  let component = ReasonReact.statelessComponent("HomeContainer");
  [@react.component]
  let make = (~number, ()) => {
    let notification =
      GlobalStateManagement.Manager.getState(Store.store).notification
      |> Belt.Option.map(_, StateManagementDef.getMessages >> List.hd)
      |> Belt.Option.getWithDefault(_, "");
    <div>
      <div> {number |> string_of_int |> React.string} </div>
      <div> {notification |> React.string} </div>
      {React.string(HomeExternalService.fetchDataHome("HOME"))}
    </div>;
  };
};

module MakeAboutContainer =
       (Store: GlobalStore, AboutExternalService: AboutExternalService) => {
  let component = ReasonReact.statelessComponent("AboutContainer");
  [@react.component]
  let make = (~string, ()) => {
    let _ = Store.store;
    <div> <div> {string |> React.string} </div> </div>;
  };
};

module MainContentRouting =
       (
         Store:
           GlobalStore with type action = StateManagementDef.action with
             type state = StateManagementDef.state,
         Service: AllExternalSercice,
       )
       : Routing.Routing => {
  // open UrlParser;

  module HomePage = MakeHomeContainer(Store, Service);
  module AboutPage = MakeAboutContainer(Store, Service);

  // let intOfStringOpt = str =>
  //   try (Some(int_of_string(str))) {
  //   | Failure(_) => None
  //   };
  type route = appRoute;
  open! UrlParser;
  let homeRoute =
    top
    / s("src")
    / s("index.html")
    |? intParamWithDefault("name", 0)
    |> toRoute(intValue => Home(intValue));
  let aboutRoute = {
    top / s("about") / string() |> toRoute(str => About(str));
  };
  let urlToRoute2 = oneOf(homeRoute ->> aboutRoute);

  /* todo url-parserで置き換えてみる */
  let urlToRoute = (url: ReasonReact.Router.url, queryParam) =>
    parseRouterUrl(urlToRoute2, url) |> Belt.Option.getExn;
  /* switch (url.path) {
     | ["src", "index.html"] =>
       Home(
         queryParam
         |> Belt.Map.String.get(_, "name")
         |> Belt.Option.flatMap(_, intOfStringOpt)
         |> Belt.Option.getWithDefault(_, 0),
       )
     | ["about"] =>
       About(Belt.Map.String.getWithDefault(queryParam, "name", "default!"))
     | route =>
       Js.Console.log(route);
       NotFound;
     }; */

  /* let subRoute = parseRouterUrl(urlToRoute2) */

  /* todm 関数合成で表現してみたい */
  let transition = route =>
    switch (route) {
    | Home(number) =>
      Utils.timePromise(3000)
      |> Js.Promise.then_(_ => <HomePage number /> |> Js.Promise.resolve)
    | About(name) =>
      <AboutPage string={Service.fetchDataAbout(name)} />
      |> Js.Promise.resolve
    | NotFound => <div> {React.string("NF")} </div> |> Js.Promise.resolve
    };
};

module App = {
  /* todo AppとStateManagementDefの棲み分けをより明確にしたい */
  include StateManagementDef;

  let initialState = {error: None, notification: None};

  let reducer = (action: action, state: state) =>
    switch (action) {
    | DetectedError(error) => {...state, error: Some(error)} ==> []
    | RefreshError => {...state, error: None} ==> []
    };

  let store = GlobalStateManagement.Manager.make(reducer, initialState);

  let renderWithStore = (~render) => render(store);
};

let showErrorModal = (error: App.error, refreshError) =>
  <div>
    {error.messages
     |> List.map(React.string)
     |> Array.of_list
     |> ReasonReact.array}
    <div onClick={_ => refreshError()} />
  </div>;

let header = () => <div> {React.string("IAM HEADER")} </div>;

module AppRoot = (MainContent: Routing.Content) => {
  // open ReactHelper;
  let blankPage = <div className="loading" />;

  [@react.component]
  let make = (~store, ()) =>
    <React.Fragment>
      {header()}
      <MainContent
        initialPage=blankPage
        onError={_ => {
          let error: App.error = {messages: ["PagePrepareError"]};
          GlobalStateManagement.Manager.dispatch(
            store,
            App.DetectedError(error),
          );
        }}
        onStartTransition={() => Js.Console.log("start_transition")}
        onFinishTransition={() => Js.Console.log("finish_transition")}
      />
    </React.Fragment>;
};

module AppRootImpl =
  AppRoot(
    (Routing.Application((MainContentRouting(App, AllExternalSerciceImpl)))),
  );

let render = () =>
  ReactDOMRe.renderToElementWithId(<AppRootImpl store=App.store />, "index");

let _ =
  () => {
    let _ = GlobalStateManagement.Manager.subscribe(App.store, render);
    render();
  };
