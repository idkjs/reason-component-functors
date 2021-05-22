[@react.component]
let make = (~title, ~children, ()) =>
  <div className="modal-header">
    {ReasonReact.cloneElement(
       title,
       ~props={"className": "modal-title"},
       [||],
     )}
    children
  </div>;
