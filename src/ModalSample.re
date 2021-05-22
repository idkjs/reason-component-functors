type action =
  | Open
  | Close;

type state = {isOpen: bool};

[@react.component]
let make = () => {
  let initialState = {isOpen: false};
  let (state, dispatch) =
    React.useReducer(
      (_, action) =>
        switch (action) {
        | Open => {isOpen: true}
        | Close => {isOpen: false}
        },
      initialState,
    );
  <div>
    <Modal
      onClickContentOutside={() => dispatch(Close)}
      hidden={!state.isOpen}
      header={
        <ModalHeader key="header" title={<h3> {React.string("MODAL!")} </h3>}>
          ...<button type_="button" className="close">
               <span> {"×" |> React.string} </span>
             </button>
        </ModalHeader>
      }
      body={
        <ModalBody key="body">
          ...<div>
               <p onClick={_ => Js.Console.log("JS")}>
                 {React.string(
                    "LINE1LINE1LINE1LINE1LINE1LINE1LINE1LINE1LINE1sLINE1LINE1LINE1LINE1LINE1LINE1LINE1LINE1LINE1s",
                  )}
               </p>
               <span> {React.string("LINE2")} </span>
             </div>
        </ModalBody>
      }
      footer={
        <ModalFooter>
          ...<button
               onClick={_ => dispatch(Close)}
               type_="button"
               className="btn btn-primary">
               {React.string("CLOSE")}
             </button>
        </ModalFooter>
      }
      size=Modal.Large
    />
    <button onClick={_ => dispatch(Open)}> {React.string("OPEN")} </button>
  </div>;
};
