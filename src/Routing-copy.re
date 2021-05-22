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

  let make:
    (

      ~initialPage: React.element,
      ~onError: Js.Promise.error => unit,
      ~onStartTransition: unit => unit,
      ~onFinishTransition: unit => unit
    ) => React.element
    // ReasonReact.componentSpec(
    //   state,
    //   state,
    //   ReasonReact.noRetainedProps,
    //   ReasonReact.noRetainedProps,
    //   action,
    // );
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


  [@react.component]
  let make =
      (~initialPage, ~onError, ~onStartTransition, ~onFinishTransition) => {


    let (state,dispatch)=React.useReducer((state,action) =>
        switch (action) {
        | StartPageLoading(element, transitionStartTime) =>{
          let _ = onStartTransition();
           {
              page: InTransition(element),
              lastTransitionTime: transitionStartTime,
            }}
        | LoadedPage(element, transitionStartTime) =>{
          switch (transitionStartTime == state.lastTransitionTime) {
      |true => {
onFinishTransition();
            {...state, page: Loaded(element)}}
| false => state
          } }
        | DetectedPageLoadError(error) =>{
onError(error);
            {...state, page: Loaded(getPageElement(state.page))}

        }},{
        page: Loaded(initialPage),
        lastTransitionTime: Js.Date.now(),
      });

   let transition = (url) => {
      open Js.Promise;
      let startTransitionTime = Js.Date.now();
      let route =
        R.urlToRoute(url, ReactHelper.Router.routeToqueryParamMap(url));
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

getPageElement(state.page);
  };

