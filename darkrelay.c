#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// use --no-pie
// Function declarations (only the necessary ones)
unsigned long long read_num();
void *malloc(size_t size);
void free(void *ptr);
size_t malloc_usable_size(void *ptr);
int printf(const char *format, ...);
int puts(const char *s);
int setvbuf(FILE *stream, char *buf, int modes, size_t n);

// Data declarations
char target[32] = {
  "WRITEME"
}; // idb

// Main function
int main(int argc, const char **argv, const char **envp)
{
    unsigned long long selected_option; // User's selection
    size_t requested_size; // Requested malloc size
    size_t usable_size; // Usable malloc size
    unsigned int malloc_request_count = 0; // Tracks malloc requests
    char *buffer_ptr; // Pointer to malloc buffer
    char *allocated_buffers[4] = {0}; // Array to store malloc-ed buffers

    setvbuf(stdout, 0LL, 2, 0LL); // Set unbuffered output
    puts("\nPatch Me First ");
    puts("House of force challenge by darkrelay");
    puts("For questions or queries, please reach out on info@darkrelay.com");
    puts("Best Of Luck\n");
    printf("puts func address leak() > %p\n", &puts);

    // First malloc to trigger heap layout
    buffer_ptr = (char *)malloc(0x88uLL);
    printf("heap address > %p\n", buffer_ptr - 16);
    free(buffer_ptr);

    while (1)
    {
        printf("\n1) AllocHeap %u/%u\n", malloc_request_count, 4);
        puts("2) SavedString");
        puts("3) Exit");
        printf("> ");
        selected_option = read_num();

        if (selected_option == 2)
        {
            printf("\ntarget: %s\n", target);
        }
        else if (selected_option == 3)
        {
            break;
        }
        else if (selected_option == 1)
        {
            if (malloc_request_count >= 4)
            {
                puts("maximum requests reached");
            }
            else
            {
                printf("size: ");
                requested_size = read_num();
                allocated_buffers[malloc_request_count] = (char *)malloc(requested_size);

                if (allocated_buffers[malloc_request_count])
                {
                    printf("data: ");
                    usable_size = malloc_usable_size(allocated_buffers[malloc_request_count]);
                    read(0, allocated_buffers[malloc_request_count++], usable_size + 8);
                }
                else
                {
                    puts("request failed");
                }
            }
        }
    }

    exit(0); // Exit cleanly
}

// Read numeric input from user
unsigned long long read_num()
{
    char input[40];
    unsigned long long input_value;

    memset(input, 0, sizeof(input));
    read(0, input, sizeof(input) - 1);
    input_value = strtoul(input, NULL, 10);
    return input_value;
}
