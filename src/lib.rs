
pub(crate) mod token;
pub(crate) mod lexer;
pub(crate) mod parser;
pub(crate) mod runner;

// ===== Imports =====
#[macro_use] extern crate thiserror;
use anyhow::Result;
use wasm_bindgen::prelude::*;

use crate::{
    lexer::Lexer,
    parser::{Parser, visualize_program},
    runner::Runner,
};
// ===================

fn visualize_lambent_program(code: String) -> Result<String> {
    let tokens = Lexer::new(code).scan_all_tokens()?;
    let program = Parser::new(tokens).parse_program()?;
    Ok(visualize_program(&program))
}

#[wasm_bindgen]
pub fn visualize_lambent_program_wasm(code: String) -> String {
    match visualize_lambent_program(code) {
        Ok(res) => res,
        Err(e) => e.to_string(),
    }
}

fn run_lambent_code(code: String) -> Result<String> {
    let tokens = Lexer::new(code).scan_all_tokens()?;
    let program = Parser::new(tokens).parse_program()?;

    let mut runner = Runner::new(program);
    runner.run()?;

    Ok(match runner.globals.get("result") {
        None => unreachable!(),
        Some(val) => format!("{:?}", val),
    })
}

#[wasm_bindgen]
pub fn run_lambent_code_wasm(code: String) -> String {
    match run_lambent_code(code) {
        Ok(res) => res,
        Err(e) => e.to_string(),
    }
}