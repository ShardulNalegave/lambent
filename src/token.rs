
#[derive(Debug, Clone, PartialEq)]
pub enum TokenKind {
    EOF,
    LeftParen,
    RightParen,
    Dot,
    Assign,
    Semicolon,
    Identifier(String),
    Number(f32),
    Add,
    Sub,
    Mul,
    Div,
    Expo,
    Lambda,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Token {
    pub kind: TokenKind,
    pub line: usize,
}
