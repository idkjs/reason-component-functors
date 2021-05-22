include FlexibleTable;

type todo = {
  text: string,
  id: string,
  age: int,
  hobby: string,
};

type todoHeader =
  | ID
  | Name
  | Age
  | Hobby;

let renderRow = (headers, data) => {
  let row =
    headers
    |> List.map(header =>
         switch (header.column) {
         | ID =>
           <td key={data.id ++ "id"} style={header.style}>
             {data.id |> React.string}
           </td>
         | Name =>
           <td key={data.id ++ "name"} style={header.style}>
             {data.text |> React.string}
           </td>
         | Age =>
           <td key={data.id ++ "age"} style={header.style}>
             {data.age |> string_of_int |> React.string}
           </td>
         | Hobby =>
           <td key={data.id ++ "hb"} style={header.style}>
             {data.hobby |> React.string}
           </td>
         }
       )
    |> Array.of_list
    |> ReasonReact.array;
  <tr key={data.id}> row </tr>;
};
let renderHeader = headers =>
  <tr>
    {headers
     |> List.map(header =>
          switch (header.column) {
          | ID =>
            <th style={header.style} key="header-id">
              {React.string("ID")}
            </th>
          | Name =>
            <th style={header.style} key="header-name">
              {React.string("Name")}
            </th>
          | Age =>
            <th style={header.style} key="header-age">
              {React.string("Age")}
            </th>
          | Hobby =>
            <th style={header.style} key="header-hobby">
              {React.string("Hobby")}
            </th>
          }
        )
     |> Array.of_list
     |> ReasonReact.array}
  </tr>;

let renderFooter = _ => React.null;

module TodoTableDef = {
  type column = todoHeader;
  type relationDataType = todo;
  type header = headerItem(column);
  type cell = tableCell(column);
  let dataRowStyle = (_, _) => "";
  let headerRowStyle = _ => "";
  let footerRowStyle = _ => "";
  let defaultHeader = header =>
    switch (header) {
    | ID => {column: ID, size: Free(200.0), isSizeFixed: false}
    | Name => {column: Name, size: Free(200.0), isSizeFixed: false}
    | Hobby => {column: Hobby, size: Free(200.0), isSizeFixed: false}
    | Age => {column: Age, size: Free(200.0), isSizeFixed: false}
    };
};

module TodoFlexibleTable = FlexibleTable(TodoTableDef);

module TodoTableSample = {
  type state = {
    headerItems: list(headerItem(todoHeader)),
    tableClassName: string,
  };
  let defaultState = {
    headerItems: TodoFlexibleTable.defaultHeaders([ID, Name, Hobby, Age]),
    tableClassName: "todo-table",
  };
  type action =
    | DeleteHeader
    | ExtendTable;
  let todo = {text: "hy!", id: "id", hobby: "hobty", age: 20};

  [@react.component]
  let make = () =>
    ReactCompat.useRecordApi({
      ...ReactCompat.component,

      reducer: (action, state) =>
        switch (action) {
        | DeleteHeader =>
          let tmpNextHeaderItems = state.headerItems |> List.tl;
          let nextHeaderItems =
            if (tmpNextHeaderItems |> List.length < 1) {
              defaultState.headerItems;
            } else {
              tmpNextHeaderItems;
            };
          UpdateWithSideEffects(
            {...state, headerItems: nextHeaderItems},
            self => {
              let _ =
                Js.Global.setTimeout(() => self.send(DeleteHeader), 1000);
              ();
            },
          );
        | ExtendTable =>
          Update({...state, tableClassName: "big-todo-table"})
        },
      didMount: self => {
        let _ = Js.Global.setTimeout(() => self.send(ExtendTable), 3000);
        ();
      },
      initialState: () => defaultState,
      render: self =>
        <TodoFlexibleTable
          tableClassName={self.state.tableClassName}
          datas=[todo]
          headerItems={self.state.headerItems}
          row=renderRow
          footer=renderFooter
          header=renderHeader
        />,
    });
};
