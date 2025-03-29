use std::collections::HashMap;
use crate::parser::{BinaryOperator, Expression, Program, UnaryOperator};

pub type Env = HashMap<String, Value>;

#[derive(Clone, Debug)]
pub enum Value {
    Number(f32),
    Closure {
        param: String,
        body: Box<Expression>,
        env: Env,
    },
}

pub struct Runner {
    pub globals: Env,
    program: Program,
}

impl Runner {
    pub fn new(program: Program) -> Self {
        Self { program, globals: HashMap::new() }
    }

    pub fn run(&mut self) {
        for stmt in &self.program.statements {
            let val = Runner::evaluate_expression(&stmt.value, &mut self.globals);
            self.globals.insert(stmt.name.clone(), val.clone());
            println!("{} = {:?}", stmt.name, val);
        }
    }

    fn evaluate_expression(expr: &Expression, env: &mut Env) -> Value {
        match expr {
            Expression::Number { value } => Value::Number(*value),
            Expression::Name { value } => {
                env.get(value)
                    .cloned()
                    .unwrap_or_else(|| panic!("Undefined variable: {}", value))
            },
            Expression::UnaryOperation { operator, operand } => {
                let v = Runner::evaluate_expression(operand, env);
                match (operator, v) {
                    (UnaryOperator::Sub, Value::Number(n)) => Value::Number(-n),
                    _ => panic!("Invalid unary operation"),
                }
            },
            Expression::BinaryOperation { lhs, operator, rhs } => {
                let left_val = Runner::evaluate_expression(lhs, env);
                let right_val = Runner::evaluate_expression(rhs, env);
                match (left_val, right_val) {
                    (Value::Number(a), Value::Number(b)) => {
                        match operator {
                            BinaryOperator::Add => Value::Number(a + b),
                            BinaryOperator::Sub => Value::Number(a - b),
                            BinaryOperator::Mul => Value::Number(a * b),
                            BinaryOperator::Div => Value::Number(a / b),
                            BinaryOperator::Expo => Value::Number(a.powf(b)),
                        }
                    },
                    _ => panic!("Binary operation on non-number values"),
                }
            },
            Expression::Abstraction { name, body } => {
                Value::Closure {
                    param: name.clone(),
                    body: body.clone(),
                    env: env.clone(),
                }
            },
            Expression::Application { lhs, rhs } => {
                let func = Runner::evaluate_expression(lhs, env);
                let arg = Runner::evaluate_expression(rhs, env);
                match func {
                    Value::Closure { param, body, env: mut closure_env } => {
                        closure_env.insert(param, arg);
                        Runner::evaluate_expression(&body, &mut closure_env)
                    },
                    _ => panic!("Attempted to apply a non-function"),
                }
            },
        }
    }
}