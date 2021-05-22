module Option = {
  let withDefault = (default, op) =>
    switch (op) {
    | Some(value) => value
    | None => default
    };
};

module String = {
  let contain = (target: string, search: string) =>
    Js.Re.(test_(fromString(search),target));
};
