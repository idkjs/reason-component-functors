type result('v, 'e) =
  | Success('v)
  | Failure('e);

module type PromiseWrapperDef = {type v;};
module PromiseWrapper = (D: PromiseWrapperDef) => {
  include Js.Promise;

  type action =
    | Completed(D.v)
    | Error(Js.Promise.error);

  type state = option(result(D.v, Js.Promise.error));

  [@react.component]
  let make =
      (
        ~promise: Js.Promise.t(D.v),
        ~whenSuccess: 'v => React.element,
        ~whenError,
        ~whenPending,
        (),
      ) =>
    ReactCompat.useRecordApi({
      ...ReactCompat.component,

      initialState: () => None,
      reducer: (action, _) =>
        switch (action) {
        | Completed(value) =>
          let state = Some(Success(value));
          Update(state);
        | Error(e) =>
          let state = Some(Failure(e));
          Update(state);
        },
      didMount: self =>
        promise
        |> then_(value => resolve(self.send(Completed(value))))
        |> catch(error => resolve(self.send(Error(error))))
        |> ignore,
      render: self =>
        switch (self.state) {
        | None => whenPending
        | Some(result) =>
          switch (result) {
          | Success(value) => whenSuccess(value)
          | Failure(e) => whenError(e)
          }
        },
    });
};
