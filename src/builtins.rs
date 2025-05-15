
// ===== Imports =====
use anyhow::Result;
use crate::{lexer::Lexer, parser::Parser, runner::Env};
// ===================

const BUILTINS: &str = include_str!("builtins.lambent");

pub fn load_builtins(globals: &mut Env) -> Result<()> {
    let toks = Lexer::new(BUILTINS.to_string()).scan_all_tokens()?;
    let prog = Parser::new(toks).parse_program()?;
    //
    Ok(())
}