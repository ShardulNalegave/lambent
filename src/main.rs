use parser::visualize_program;

pub mod token;
pub mod lexer;
pub mod parser;

fn main() {
    let code = "result = L a.(a+1) 5";

    let mut l = lexer::Lexer::new(code.to_string());
    let tokens = l.scan_all_tokens();
    let mut p = parser::Parser::new(tokens);
    let prog = p.parse_program();
    println!("{}", visualize_program(&prog));
}
