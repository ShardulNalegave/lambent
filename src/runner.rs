
// ===== Imports =====
use std::collections::HashMap;
use crate::parser::{BinaryOperator, Expression, Program, UnaryOperator};
// ===================

#[derive(Error, Debug)]
pub enum RunnerError {
    #[error("Undefined variable: {name:?}")]
    UndefinedVariable { name: String },
    #[error("Binary operation performed on non-number values")]
    BinaryOperationOnNonNumbers,
    #[error("Attempted to apply a non-function")]
    ApplyNonFunction,
}

pub type Env = HashMap<String, Value>;

#[derive(Clone)]
pub enum Value {
    Number(f32),
    Function {
        param: String,
        body: Box<Expression>,
        env: Env,
    },
    Thunk {
        expr: Box<Expression>,
        env: Env,
    },
}

impl std::fmt::Debug for Value {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Number(val) => write!(f, "{}", val),
            Self::Function { .. } => write!(f, "<Function>"),
            Self::Thunk { .. } => write!(f, "<Thunk>"),
        }
    }
}

pub struct Runner {
    pub globals: Env,
    program: Program,
}

impl Runner {
    pub fn new(program: Program) -> Self {
        Self { program, globals: HashMap::new() }
    }

    pub fn run(&mut self) -> Result<(), RunnerError> {
        for stmt in &self.program.statements {
            let val = Runner::evaluate_expression(&stmt.value, &mut self.globals)?;
            self.globals.insert(stmt.name.clone(), val.clone());
        }

        Ok(())
    }

    fn evaluate_expression(expr: &Expression, env: &mut Env) -> Result<Value, RunnerError> {
        Ok(match expr {
            Expression::Number { value } => Value::Number(*value),
            Expression::Name { value } => {
                let v = env.get(value)
                    .cloned()
                    .ok_or(RunnerError::UndefinedVariable { name: value.clone() })?;

                match v {
                    Value::Number(_) | Value::Function { .. } => v,
                    Value::Thunk { expr: thunk_expr, env: mut thunk_env } => {
                        let forced = Runner::evaluate_expression(&thunk_expr, &mut thunk_env)?;
                        forced
                    }
                }
            },
            Expression::UnaryOperation { operator, operand } => {
                let v = Runner::evaluate_expression(operand, env)?;
                match (operator, v) {
                    (UnaryOperator::Sub, Value::Number(n)) => Value::Number(-n),
                    _ => panic!("Invalid unary operation"),
                }
            },
            Expression::BinaryOperation { lhs, operator, rhs } => {
                let left_val = Runner::evaluate_expression(lhs, env)?;
                let right_val = Runner::evaluate_expression(rhs, env)?;

                let true_func = env.get("true")
                    .expect("'true' not found, builtins missing");
                let false_func = env.get("false")
                    .expect("'false' not found, builtins missing");

                match (left_val, right_val) {
                    (Value::Number(a), Value::Number(b)) => {
                        match operator {
                            BinaryOperator::Add => Value::Number(a + b),
                            BinaryOperator::Sub => Value::Number(a - b),
                            BinaryOperator::Mul => Value::Number(a * b),
                            BinaryOperator::Div => Value::Number(a / b),
                            BinaryOperator::Expo => Value::Number(a.powf(b)),

                            BinaryOperator::Equal => if a == b { true_func.clone() } else { false_func.clone() },
                            BinaryOperator::NotEqual => if a != b { true_func.clone() } else { false_func.clone() },
                            BinaryOperator::LessThan => if a < b { true_func.clone() } else { false_func.clone() },
                            BinaryOperator::LessThanOrEqual => if a <= b { true_func.clone() } else { false_func.clone() },
                            BinaryOperator::GreaterThan => if a > b { true_func.clone() } else { false_func.clone() },
                            BinaryOperator::GreaterThanOrEqual => if a >= b { true_func.clone() } else { false_func.clone() },
                        }
                    },
                    _ => return Err(RunnerError::BinaryOperationOnNonNumbers),
                }
            },
            Expression::Function { name, body } => {
                Value::Function {
                    param: name.clone(),
                    body: body.clone(),
                    env: env.clone(),
                }
            },
            Expression::Application { lhs, rhs } => {
                let func_val = Runner::evaluate_expression(lhs, env)?;
                match func_val {
                    Value::Function { param, body, mut env } => {
                        // 2) Instead of eagerly doing `evaluate_expression(rhs, env)`,
                        //    insert a Thunk so the function body can force it later.
                        let arg_thunk = Value::Thunk {
                            expr: Box::new((**rhs).clone()),
                            env: env.clone(),   // capture the caller's environment
                        };

                        env.insert(param, arg_thunk);
                        Runner::evaluate_expression(&body, &mut env)?
                    }
                    _ => return Err(RunnerError::ApplyNonFunction),
                }
            },
        })
    }
}