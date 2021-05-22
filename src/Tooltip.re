type action =
  | Open
  | Close;

type position =
  | Left
  | Right
  | Down
  | Up;

type size =
  | Small
  | Medium
  | Large
  | XLarge
  | Fit;

let positionToAttrString = position =>
  switch (position) {
  | Left => "left"
  | Right => "right"
  | Down => "down"
  | Up => "up"
  };

let sizeToAttrString = size =>
  switch (size) {
  | Small => "small"
  | Medium => "medium"
  | Large => "large"
  | XLarge => "xlarge"
  | Fit => "fit"
  };

type state = {isOpen: bool};

[@react.component]
let make = (~position, ~size, ~text: string, ~parent, ()) =>
  ReactCompat.useRecordApi({
    ...ReactCompat.component,

    initialState: () => {isOpen: false},
    reducer: (action, _) =>
      switch (action) {
      | Open => Update({isOpen: true})
      | Close => Update({isOpen: false})
      },
    render: self => {
      let tooltip =
        ReasonReact.cloneElement(
          parent,
          ~props={
            "data-balloon-length": sizeToAttrString(size),
            "data-balloon": text,
            "data-balloon-pos": positionToAttrString(position),
            "onMouseOver": _ => self.send(Open),
            "onMouseLeave": _ => self.send(Close),
          },
          [||],
        );
      tooltip;
    },
  });
