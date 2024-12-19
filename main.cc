#include "parse.cc"

// top ::= definition | external | expression | ';'
static void main_loop() {
	while (1) {
		std::cerr << "ready> ";

		switch(curr_tok) {
		case static_cast<int>(Token::eof):
			return;
		case static_cast<int>(Token::def):
			handle_definition();
			break;
		case static_cast<int>(Token::ext):
			handle_extern();
			break;
		case ';':
			get_next_token();
			break;
		default:
			handle_top_level_expr();
			break;
		}
	}
}

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

int main() {
	std::cerr << "ready> ";
	get_next_token();
	main_loop();
}