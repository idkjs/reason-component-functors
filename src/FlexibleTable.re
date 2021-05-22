include FlexibleTableHeader;

type element;

[@bs.send]
external addEventListener: (Dom.element, string, unit => unit) => unit =
  "addEventListener";

type tableCell('column) = {
  column: 'column,
  style: ReactDOMRe.style,
};

module type TableDef = {
  type column;
  type relationDataType;
  type header = headerItem(column);
  type cell = tableCell(column);
  let dataRowStyle: (list(header), relationDataType) => string;
  let headerRowStyle: list(header) => string;
  let footerRowStyle: list(header) => string;

  let defaultHeader: column => header;
};

module FlexibleTable = (T: TableDef) => {
  type state = {tableWidth: float};
  type action =
    | DetectedTableSize(float);
  let defaultHeaders = headerColumns =>
    headerColumns |> List.map(T.defaultHeader);
  let headerItemToCell = (header: T.header): T.cell => {
    let width = getWidthSize(header.size);
    let px = "px";
    {
      column: header.column,
      style: ReactDOMRe.Style.make(~width={j|$width$px|j}, ()),
    };
  };

  [@react.component]
  let make =
      (
        ~datas: list('data),
        ~headerItems: list(T.header),
        ~row: (list(T.cell), 'data) => React.element,
        ~header: list(T.cell) => React.element,
        ~footer: list(T.cell) => React.element,
        ~tableClassName: string,
        (),
      ) => {
    // let initialState = {
    //   tableWidth: 0.0
    // };
    // this is cool. maybe unbox it to make it work?
    // let (tableWidth, setTableWidth) = React.useState(_ => DetectedTableSize(0.0));
    let (tableWidth, setTableWidth) = React.useState(_ => 0.0);
    let (state, dispatch) =
      React.useReducer(
        (state, action) =>
          switch (action) {
          | DetectedTableSize(tableWidth) => tableWidth
          },
        [@reason.preserve_braces] 0.0,
      );
    let theRef: React.ref(Js.Nullable.t(Dom.element)) =
      React.useRef(Js.Nullable.null);
    let apperTable = ref => {
      let domOpt = Js.Nullable.toOption(ref);
      let tableDomToAction = tableDom => {
        let newtableWidth: float =
          ReactDOMRe.domElementToObj(tableDom)##clientWidth;
        // this is cool. maybe unbox it to make it work?
        // if (newtableWidth != DetectedTableSize(tableWidth)) {
        //   DetectedTableSize(tableWidth) |> dispatch;
        // };
        if (newtableWidth != tableWidth) {
          DetectedTableSize(tableWidth) |> dispatch;
        };
      };
      switch (domOpt) {
      | Some(dom) =>
        let tableDom = dom;

        addEventListener(tableDom, "transitionend", () =>
          tableDomToAction(dom)
        );

        tableDomToAction(tableDom);
      | None => ()
      };
    };
    React.useEffect1(
      () => {
        apperTable(theRef.current);
        None;
      },
      [|theRef|],
    );
    let cells =
      headerItems
      |> getWidthSizeByTableWidthSize(tableWidth)
      |> List.map(headerItemToCell);
    let bodyRows =
      datas |> List.map(row(cells)) |> Array.of_list |> ReasonReact.array;

    let header = cells |> header;
    let footer = cells |> footer;
    <table className=tableClassName ref={ReactDOM.Ref.domRef(theRef)}>
      <thead> header </thead>
      <tbody> bodyRows </tbody>
      <tfoot> footer </tfoot>
    </table>;
  };
};
