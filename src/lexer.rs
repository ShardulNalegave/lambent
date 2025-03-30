
// ===== Imports =====
use crate::token::{Token, TokenKind};
// ===================

#[derive(Error, Debug, Clone)]
pub enum LexerError {
    #[error("Unexpected character '{character}' on line {line}")]
    UnexpectedCharacter { character: char, line: usize },
}

pub struct Lexer {
    pos: usize,
    source: String,
    line: usize,
}

impl Lexer {
    pub fn new(source: String) -> Self {
        Self { source, line: 1, pos: 0 }
    }

    pub fn scan_all_tokens(&mut self) -> Result<Vec<Token>, LexerError> {
        let mut tokens = vec![];
        loop {
            let tok = self.scan_token()?;
            if tok.kind == TokenKind::EOF {
                tokens.push(tok);
                break;
            }

            tokens.push(tok);
        }
        Ok(tokens)
    }
    
    pub fn scan_token(&mut self) -> Result<Token, LexerError> {
        self.skip_whitespace();
        self.skip_comment();

        // Early return for identifiers, keywords and numbers
        if let Some(c) = self.peek() {
            if c.is_alphabetic() {
                return Ok(self.read_identifier_or_keyword());
            } else if c.is_digit(10) {
                return Ok(self.read_number());
            }
        }

        let c= self.advance();
        Ok(self.make_token(match c {
            None => TokenKind::EOF,
            Some('(') => TokenKind::LeftParen,
            Some(')') => TokenKind::RightParen,
            Some('.') => TokenKind::Dot,
            Some('+') => TokenKind::Add,
            Some('-') => TokenKind::Sub,
            Some('*') => TokenKind::Mul,
            Some('/') => TokenKind::Div,
            Some('^') => TokenKind::Expo,
            Some('=') => TokenKind::Assign,
            Some(';') => TokenKind::Semicolon,

            Some(c) => return Err(LexerError::UnexpectedCharacter { character: c, line: self.line }),
        }))
    }

    fn make_token(&self, kind: TokenKind) -> Token {
        Token { kind, line: self.line }
    }

    fn advance(&mut self) -> Option<char> {
        let c = self.source.chars().nth(self.pos);
        self.pos += 1;
        c
    }

    fn peek(&self) -> Option<char> {
        self.source.chars().nth(self.pos)
    }

    fn skip_whitespace(&mut self) {
        loop {
            match self.peek() {
                Some(' ' | '\r' | '\t') => {
                    self.advance();
                },
                Some('\n') => {
                    self.advance();
                    self.line += 1;
                },
                _ => break,
            }
        }
    }

    fn skip_comment(&mut self) {
        if let Some('#') = self.peek() {
            while let Some(c) = self.peek() {
                self.advance();
                if c == '\n' {
                    self.line += 1;
                    break;
                }
            }
        }
    }

    fn read_identifier_or_keyword(&mut self) -> Token {
        let mut ident = String::new();
        while let Some(c) = self.peek() {
            if c.is_alphanumeric() || c == '_' {
                ident.push(c);
                self.advance();
            } else {
                break;
            }
        }

        match ident.as_str() {
            "L" => self.make_token(TokenKind::Lambda),
            _ => self.make_token(TokenKind::Identifier(ident)),
        }
    }

    fn read_number(&mut self) -> Token {
        let mut num = String::new();
        let mut has_decimal = false;

        while let Some(c) = self.peek() {
            if c.is_digit(10) {
                num.push(c);
                self.advance();
            } else if c == '.' && !has_decimal {
                num.push(c);
                has_decimal = true;
                self.advance();
            } else {
                break;
            }
        }

        let num_float = num
            .parse::<f32>()
            .expect("Could not parse number");
        self.make_token(TokenKind::Number(num_float))
    }
}