
pub mod token;
pub mod lexer;
pub mod parser;

fn main() {
    let code = "() 5.24 3 . abcd.a_d";

    let mut l = lexer::Lexer::new(code.to_string());
    let tokens = l.scan_all_tokens();
    let mut p = parser::Parser::new(tokens);
}
