open Helpers;

module type Def = {
  type entity;
  let findEntities: string => Js.Promise.t(list(entity));
};

module IncrementalSearch = (D: Def) => {
  include Js.Promise;
  type action =
    | InputText(string)
    | CompletedFindEntities(list(D.entity))
    | Dequeue;

  type state = {
    text: string,
    entities: list(D.entity),
    queueGetEntitesCommand: list(string),
  };

  [@react.component]
  let make =
      (
        ~defaultText: option(string)=?,
        ~searchDelay: option(float)=?,
        ~searchResultView: list(D.entity) => React.element,
        (),
      ) =>
    ReactCompat.useRecordApi({
      ...ReactCompat.component,

      initialState: () => {
        text: Option.withDefault("", defaultText),
        entities: [],
        queueGetEntitesCommand: [],
      },
      didMount: self => self.send(InputText(self.state.text)),
      reducer: (action, state) =>
        switch (action) {
        | InputText(text) =>
          let delay = Option.withDefault(0.0, searchDelay);
          UpdateWithSideEffects(
            {
              ...state,
              text,
              queueGetEntitesCommand:
                List.concat([state.queueGetEntitesCommand, [text]]),
            },
            self => {
              let _ =
                Js.Global.setTimeout(
                  () => self.send(Dequeue),
                  delay |> floor |> int_of_float,
                );
              ();
            },
          );
        | CompletedFindEntities(entities) => Update({...state, entities})
        | Dequeue =>
          switch (state.queueGetEntitesCommand) {
          | [command] =>
            UpdateWithSideEffects(
              {...state, queueGetEntitesCommand: []},
              self =>
                D.findEntities(command)
                |> then_(entities =>
                     resolve(self.send(CompletedFindEntities(entities)))
                   )
                |> ignore,
            )
          | [_, ...tail] => Update({...state, queueGetEntitesCommand: tail})
          | [] => NoUpdate
          }
        },
      render: self => {
        let text = self.state.text;
        let onInputText = e => {
          let text = ReactEvent.Form.target(e)##value;
          self.send(InputText(text));
        };
        <div>
          <input onInput={e => onInputText(e)} value=text />
          {searchResultView(self.state.entities)}
        </div>;
      },
    });
};
