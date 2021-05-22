module type Routing_ = {
  type route;
  let urlToRoute: ReasonReact.Router.url => route;
  let transition: route => Js.Promise.t(React.element);
};

module type Routing = {
  type route;
  let urlToRoute:
    (ReasonReact.Router.url, Belt.Map.String.t(string)) => route;
  let transition: route => Js.Promise.t(React.element);
};

module type Content = {
  type page;

  type state;

  type action;
  [@react.component]
  let make:
    (
      ~initialPage: React.element,
      ~onError: Js.Promise.error => unit,
      ~onStartTransition: unit => unit,
      ~onFinishTransition: unit => unit
    ) =>
    React.element;
};

module Application = (R: Routing) : Content => {
  type page =
    | InTransition(React.element)
    | Loaded(React.element);

  let getPageElement = page => {
    let InTransition(pageElement) | Loaded(pageElement) = page;
    pageElement;
  };

  type state = {
    page,
    lastTransitionTime: float,
  };

  type action =
    | StartPageLoading(React.element, float)
    | LoadedPage(React.element, float)
    | DetectedPageLoadError(Js.Promise.error);

  // let component = ReasonReact.reducerComponent("application");

  [@react.component]
  let make = (~initialPage, ~onError, ~onStartTransition, ~onFinishTransition) => {
    let (state, dispatch) =
      React.useReducer(
        (state, action) =>
          switch (action) {
          | StartPageLoading(element, transitionStartTime) => {
              page: InTransition(element),
              lastTransitionTime: transitionStartTime,
            }
          // UpdateWithSideEffects(
          //   {
          //     page: InTransition(element),
          //     lastTransitionTime: transitionStartTime,
          //   },
          //   _ => onStartTransition(),
          // )
          | LoadedPage(element, transitionStartTime) =>
            transitionStartTime == state.lastTransitionTime
              ? {...state, page: Loaded(element)} : state
          // if (transitionStartTime == state.lastTransitionTime) {
          //   UpdateWithSideEffects(
          //     {...state, page: Loaded(element)},
          //     _ => onFinishTransition(),
          //   );
          // } else {
          //   NoUpdate;
          // }
          | DetectedPageLoadError(error) => {
              ...state,
              page: Loaded(getPageElement(state.page)),
            }
          // UpdateWithSideEffects(
          //   {...state, page: Loaded(getPageElement(state.page))},
          //   _ => onError(error),
          // )
          },
        {page: Loaded(initialPage), lastTransitionTime: Js.Date.now()},
      );
    let transition = url => {
      open Js.Promise;
      let startTransitionTime = Js.Date.now();
      let route =
        R.urlToRoute(url, ReactHelper.Router.routeToqueryParamMap(url));
      onStartTransition();
      dispatch(
        StartPageLoading(getPageElement(state.page), startTransitionTime),
      );
      R.transition(route)
      |> then_(element =>
           LoadedPage(element, startTransitionTime) |> dispatch |> resolve
         )
      |> catch(error => {
           dispatch(DetectedPageLoadError(error));
           resolve();
         })
      |> ignore;
    };

    React.useEffect0(() => {
      let id = ReasonReact.Router.watchUrl(transition);
      ignore(transition(ReasonReact.Router.dangerouslyGetInitialUrl()));
      Some(() => ReasonReact.Router.unwatchUrl(id));
    });

    getPageElement(state.page);
  };
};
