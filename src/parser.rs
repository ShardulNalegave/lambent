use crate::token::{Token, TokenKind};

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum UnaryOperator {
    Sub,
}

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum BinaryOperator {
    Add,
    Sub,
    Mul,
    Div,
    Expo,
}

#[derive(Clone, Debug, PartialEq)]
pub enum Expression {
    Name { value: String },
    Number { value: f32 },
    Abstraction {
        name: String,
        body: Box<Expression>,
    },
    Application {
        lhs: Box<Expression>,
        rhs: Box<Expression>,
    },
    BinaryOperation {
        lhs: Box<Expression>,
        operator: BinaryOperator,
        rhs: Box<Expression>,
    },
    UnaryOperation {
        operator: UnaryOperator,
        operand: Box<Expression>
    },
}

#[derive(Clone, Debug, PartialEq)]
pub struct Statement {
    pub name: String,
    pub value: Expression,
}

#[derive(Clone, Debug, PartialEq)]
pub struct Program {
    pub statements: Vec<Statement>,
}

pub struct Parser {
    pos: usize,
    tokens: Vec<Token>,
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Self {
        Self {tokens, pos: 0}
    }

    pub fn advance_token(&mut self) -> Option<&Token> {
        let tok = self.tokens.get(self.pos);
        self.pos += 1;
        tok
    }

    pub fn peek_token(&self) -> Option<&Token> {
        self.tokens.get(self.pos)
    }

    pub fn parse_program(&mut self) -> Program {
        let mut statements = Vec::new();
        while let Some(token) = self.peek_token() {
            if let TokenKind::EOF { .. } = token.kind {
                break;
            }
            statements.push(self.parse_statement());
        }
        Program { statements }
    }

    pub fn parse_statement(&mut self) -> Statement {
        let token = self.advance_token().expect("Expected a token found None");
        let name = match &token.kind {
            TokenKind::Identifier(ident) => ident.clone(),
            _ => panic!("Expected identifier at start of statement, got {:?}", token),
        };

        let token = self.advance_token().expect("Expected '=' after identifier");
        match &token.kind {
            TokenKind::Assign => {},
            _ => panic!("Expected '=' token after identifier, got {:?}", token),
        }

        let value = self.parse_expression();
        Statement { name, value }
    }

    pub fn parse_expression(&mut self) -> Expression {
        self.parse_expression_bp(0)
    }

    fn parse_expression_bp(&mut self, min_bp: u8) -> Expression {
        let mut lhs = match self.peek_token() {
            Some(token) => match &token.kind {
                TokenKind::Number(value) => {
                    let expr = Expression::Number { value: *value };
                    self.advance_token();
                    expr
                }
                
                TokenKind::Identifier(value) => {
                    let expr = Expression::Name { value: value.clone() };
                    self.advance_token();
                    expr
                }
                
                TokenKind::LeftParen => {
                    self.advance_token();
                    let expr = self.parse_expression_bp(0);
                    let next = self.advance_token().expect("Expected ')' after expression");
                    match next.kind {
                        TokenKind::RightParen => expr,
                        _ => panic!("Expected ')' after expression, got {:?}", next),
                    }
                }
                
                TokenKind::Lambda => {
                    self.advance_token();
                    let param_token = self.advance_token().expect("Expected identifier after lambda");
                    let param = match &param_token.kind {
                        TokenKind::Identifier(value) => value.clone(),
                        _ => panic!("Expected identifier after lambda token, got {:?}", param_token),
                    };
                    
                    let dot_token = self.advance_token().expect("Expected '.' after lambda parameter");
                    match dot_token.kind {
                        TokenKind::Dot => {},
                        _ => panic!("Expected '.' after lambda parameter, got {:?}", dot_token),
                    }
                    let body = self.parse_expression_bp(51);
                    Expression::Abstraction { name: param, body: Box::new(body) }
                }
                
                TokenKind::Sub => {
                    self.advance_token();
                    let right_bp = 50;
                    let rhs = self.parse_expression_bp(right_bp);
                    Expression::UnaryOperation {
                        operator: UnaryOperator::Sub,
                        operand: Box::new(rhs),
                    }
                }
                _ => panic!("Unexpected token in prefix position: {:?}", token),
            },
            None => panic!("Expected expression, found EOF"),
        };

        
        loop {
            let op = match self.peek_token() {
                Some(token) => token,
                None => break,
            };

            let (lbp, rbp, op_kind) = {
                if let TokenKind::Add = op.kind {
                    (10, 11, Some("binary"))
                } else if let TokenKind::Sub = op.kind {
                    (10, 11, Some("binary"))
                } else if let TokenKind::Mul = op.kind {
                    (20, 21, Some("binary"))
                } else if let TokenKind::Div = op.kind {
                    (20, 21, Some("binary"))
                } else if let TokenKind::Expo = op.kind {
                    (30, 29, Some("binary"))
                }
                else if matches!(op.kind,
                        TokenKind::Number { .. }
                        | TokenKind::Identifier { .. }
                        | TokenKind::LeftParen
                        | TokenKind::Lambda
                    ) {
                    (50, 51, Some("application"))
                } else {
                    break;
                }
            };

            if lbp < min_bp {
                break;
            }

            if op_kind == Some("application") {
                let rhs = self.parse_expression_bp(rbp);
                lhs = Expression::Application {
                    lhs: Box::new(lhs),
                    rhs: Box::new(rhs),
                };
                continue;
            }

            let op_token = self.advance_token().unwrap();
            let operator = match op_token.kind {
                TokenKind::Add => BinaryOperator::Add,
                TokenKind::Sub => BinaryOperator::Sub,
                TokenKind::Mul => BinaryOperator::Mul,
                TokenKind::Div => BinaryOperator::Div,
                TokenKind::Expo => BinaryOperator::Expo,
                _ => unreachable!(),
            };

            let rhs = self.parse_expression_bp(rbp);
            lhs = Expression::BinaryOperation {
                lhs: Box::new(lhs),
                operator,
                rhs: Box::new(rhs),
            };
        }

        lhs
    }
}

pub fn visualize_program(program: &Program) -> String {
    let mut output = String::new();
    for stmt in &program.statements {
        output.push_str(&format!("Statement: {}\n", stmt.name));
        output.push_str(&visualize_expression(&stmt.value, 1));
    }
    output
}

fn visualize_expression(expr: &Expression, indent: usize) -> String {
    let mut output = String::new();
    let indent_str = "  ".repeat(indent);

    match expr {
        Expression::Name { value } => {
            output.push_str(&format!("{}Name: {}\n", indent_str, value));
        }
        Expression::Number { value } => {
            output.push_str(&format!("{}Number: {}\n", indent_str, value));
        }
        Expression::Abstraction { name, body } => {
            output.push_str(&format!("{}Abstraction (param: {})\n", indent_str, name));
            output.push_str(&visualize_expression(body, indent + 1));
        }
        Expression::Application { lhs, rhs } => {
            output.push_str(&format!("{}Application:\n", indent_str));
            output.push_str(&visualize_expression(lhs, indent + 1));
            output.push_str(&visualize_expression(rhs, indent + 1));
        }
        Expression::BinaryOperation { lhs, operator, rhs } => {
            output.push_str(&format!("{}BinaryOperation: {:?}\n", indent_str, operator));
            output.push_str(&visualize_expression(lhs, indent + 1));
            output.push_str(&visualize_expression(rhs, indent + 1));
        }
        Expression::UnaryOperation { operator, operand } => {
            output.push_str(&format!("{}UnaryOperation: {:?}\n", indent_str, operator));
            output.push_str(&visualize_expression(operand, indent + 1));
        }
    }
    output
}
