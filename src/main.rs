
pub mod token;
pub mod lexer;
pub mod parser;
pub mod runner;

// ===== Imports =====
#[macro_use] extern crate thiserror;
use std::{fs, path::PathBuf};
use clap::Parser as ClapParser;

use crate::{
    lexer::Lexer,
    parser::{Parser, visualize_program},
    runner::Runner,
};
// ===================

#[derive(ClapParser, Debug, Clone)]
#[command(version)]
pub struct Args {
    /// Input file
    file: PathBuf,
    /// Show AST generated from source code
    #[arg(short, long, default_value_t=false)]
    ast: bool,
}

fn main() {
    let args = Args::parse();
    let code = fs::read_to_string(&args.file).expect("Could not read file");

    let tokens = match Lexer::new(code).scan_all_tokens() {
        Ok(tokens) => tokens,
        Err(e) => {
            eprintln!("{}", e.to_string());
            return;
        },
    };

    let program = match Parser::new(tokens).parse_program() {
        Ok(program) => program,
        Err(e) => {
            eprintln!("{}", e.to_string());
            return;
        },
    };

    if args.ast {
        let visualisation = visualize_program(&program);
        println!("{}", visualisation);
    }

    let mut runner = Runner::new(program);
    match runner.run() {
        Ok(_) => {},
        Err(e) => {
            eprint!("{}", e.to_string());
            return;
        },
    }

    match runner.globals.get("result") {
        None => unreachable!(),
        Some(val) => println!("{:?}", val),
    }
}
