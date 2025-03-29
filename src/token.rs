
#[derive(Debug, Clone, PartialEq)]
pub enum TokenKind {
    EOF,
    LeftParen,
    RightParen,
    Dot,
    Identifier(String),
    Number(f32),
    Add,
    Sub,
    Mul,
    Div,
    Expo,
    Assign,
    Lambda,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Token {
    pub kind: TokenKind,
    pub line: u32,
}
