#include "stdio.h"
#include "scanner.h"

int main() {
    scanner_init("L result.(+ result 1) 5");
    token_t tok = scanner_scan_token();
    while (tok.type != TOKEN_EOF) {
        printf("%d ", tok.type);
        for (int i = 0; i < tok.len; i++) {
            printf("%c", tok.start[i]);
        }
        printf("\n");
        tok = scanner_scan_token();
    }

    return 0;
}