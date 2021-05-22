module type Def = {
  type resource;
  type loadResourceArg;
  type t;
  let componentName: string;
  let initialize: resource => t;
  let render: t => React.element;
  let loadResource: loadResourceArg => Js.Promise.t(resource);
};

module Make =
       (
         D: Def,
         Pdef: PromiseWrapper.PromiseWrapperDef with type v = D.resource,
       ) => {
  module ResourcePromiseWrapper = PromiseWrapper.PromiseWrapper(Pdef);

  type resource = Js.Promise.t(D.resource);

  let ifError = <div> {React.string("ERROR")} </div>;
  let ifPending = <div> {React.string("Loading")} </div>;
  let ifSuccess = resource => resource |> D.initialize |> D.render;

  let component = ReasonReact.statelessComponent(D.componentName);
  [@react.component]
  let make = (~loadResourceArg: D.loadResourceArg, ()) =>
    <ResourcePromiseWrapper
      promise={D.loadResource(loadResourceArg)}
      whenSuccess=ifSuccess
      whenError={_ => ifError}
      whenPending=ifPending
    />;
};

module Page = (D: Def) => {
  let initialize = (loadResourceArg: D.loadResourceArg) =>
    Js.Promise.(
      D.loadResource(loadResourceArg)
      |> then_(resource => resource |> D.initialize |> D.render |> resolve)
    );
};
