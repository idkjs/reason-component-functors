/* todo textの長さチェック、数字に変換できるかチェック、option型にチェックできるやつ
      variantは型構築子一つならswitchいらずか確認
   */
type validator('e, 's) =
  | Validator('s => list('e));

let fromErrors = (getErrors: 'subject => list('e)): validator('e, 'subject) =>
  Validator(getErrors);

let all = (validators: list(validator('e, 's))): validator('e, 's) => {
  let allGetErrors =
    validators
    |> List.map(validator =>
         switch (validator) {
         | Validator(getErrors) => getErrors
         }
       );

  let validateLogic = subject =>
    allGetErrors |> List.map(getError => getError(subject)) |> List.flatten;

  Validator(validateLogic);
};

let first = (validators: list(validator('e, 's))) => {
  let allValidation = all(validators);

  let validateLogic = subject =>
    switch (allValidation) {
    | Validator(getErrors) =>
      try (getErrors(subject)) {
      | Failure(_) => []
      }
    };

  Validator(validateLogic);
};

let run = (validator, source) =>
  switch (validator) {
  | Validator(execute) => source |> execute
  };
