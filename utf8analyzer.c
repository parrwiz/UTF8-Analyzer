#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>



int32_t is_ascii(char str[]){
    for(int i=0; str[i]!='\0'; i++){
        if((unsigned char)str[i]>127){
            return 0;
        }
    }
    return 1;
}

int32_t capitalize_ascii(char str[]){
    int count=0;
    for(int i=0; str[i]!='\0'; i+=1){
        if((unsigned char)str[i]>=97 && (unsigned char)str[i]<= 122){
            str[i]=str[i]-32;
            count+=1;
        }
    }
    return count;
}

int32_t width_from_start_byte(char start_byte){
    unsigned char byte = (unsigned char)start_byte;
    if(byte<=0b01111111){
        return 1;
    }else if (byte>=0b11000000 && byte<0b11100000){
        return 2;
    }else if(byte>=0b11100000 && byte<0b11110000){
        return 3;

    }else if(byte>=0b11110000&&byte<0b11110100)
    {
       return 4;
    }else{
        return -1;
    }
}

int32_t utf8_strlen(char str[]){
    int32_t width=0;
    int32_t length =0;
    for(int i=0; str[i]!='\0';){
        width = width_from_start_byte(str[i]);
        if(width==-1){
            return -1;
        }
        length++;
         i+=width;
    }
    return length;
}
int32_t codepoint_index_to_byte_index(char str[], int32_t cpi) {
    if (cpi < 0) return -1;
    if (cpi == 0) return 0;
    
    int32_t current_cpi = 0;
    int32_t i = 0;
    
    while (str[i] != '\0' && current_cpi < cpi) {
        int32_t width = width_from_start_byte(str[i]);
        if (width == -1) return -1;
        
        i += width;
        current_cpi++;
    }
    
    return (current_cpi == cpi) ? i : -1;
}
void utf8_substring(char str[], int32_t cpi_start, int32_t cpi_end, char result[])
{
    if(cpi_start<0){
	    return;
    }
    int32_t str_length= utf8_strlen(str);
    if(cpi_end>str_length){
	    cpi_end=str_length;
    }
    if(cpi_end<=cpi_start){
	    result[0]='\0';
	    return;
    }

     int32_t bi_start=codepoint_index_to_byte_index(str,cpi_start);
    int32_t bi_end=codepoint_index_to_byte_index(str, cpi_end);

    if(bi_start==-1 || bi_end==-1){
        return;
    }
    int counter=0;
    for(int i=bi_start; i<bi_end; i++){
        result[counter]=str[i];
      counter+=1;
    }
   
    result[counter]='\0';

}


int32_t codepoint_at(char str[], int32_t cpi) {
    if (cpi < 0) return -1;
    
    int32_t byte_index = codepoint_index_to_byte_index(str, cpi);
    if (byte_index < 0) return -1;
    
    unsigned char byte = (unsigned char)str[byte_index];
    int32_t codepoint = 0;
    
    if (byte <= 0x7F) {
        codepoint = byte;
    } else if ((byte & 0xE0) == 0xC0 && str[byte_index + 1] != '\0') {
        codepoint = ((byte & 0x1F) << 6) | (str[byte_index + 1] & 0x3F);
    } else if ((byte & 0xF0) == 0xE0 && str[byte_index + 1] != '\0' && str[byte_index + 2] != '\0') {
        codepoint = ((byte & 0x0F) << 12) |
                    ((str[byte_index + 1] & 0x3F) << 6) |
                    (str[byte_index + 2] & 0x3F);
    } else if ((byte & 0xF8) == 0xF0 && str[byte_index + 1] != '\0' &&
               str[byte_index + 2] != '\0' && str[byte_index + 3] != '\0') {
        codepoint = ((byte & 0x07) << 18) |
                    ((str[byte_index + 1] & 0x3F) << 12) |
                    ((str[byte_index + 2] & 0x3F) << 6) |
                    (str[byte_index + 3] & 0x3F);
    } else {
        return -1;
    }
    
    return codepoint;
}


int32_t is_animal_emoji_at( char str[], int32_t cpi) {
    int32_t codepoint = codepoint_at(str, cpi);
    return (codepoint >= 0b00011111010000000000 && codepoint <= 0b00011111010000111110) ||
           (codepoint >= 0b00011111100110000000 && codepoint <= 0b00011111100110000100);
}


//Implementation of next_utf8_char
void next_utf8_char(char str[], int32_t cpi, char result[]) {
    result[0] = '\0';  

    int32_t total_codepoints = utf8_strlen(str);
    
    if (cpi >= total_codepoints) {
        return;      }

    int32_t current_codepoint = codepoint_at(str, cpi);
    if (current_codepoint == -1) {
        return;  
    }

    int32_t next_codepoint = current_codepoint + 1;

    if (next_codepoint <= 0x7F) {
        // 1 byte
        result[0] = (char)next_codepoint;
        result[1] = '\0';
    } else if (next_codepoint <= 0x7FF) {
        // 2 bytes
        result[0] = 0b11000000 | (next_codepoint >> 6);
        result[1] = 0b10000000 | (next_codepoint & 0b00111111);
        result[2] = '\0';
    } else if (next_codepoint <= 0xFFFF) {
        // 3 bytes
        result[0] = 0b11100000 | (next_codepoint >> 12);
        result[1] = 0b10000000 | ((next_codepoint >> 6) & 0b00111111);
        result[2] = 0b10000000 | (next_codepoint & 0b00111111);
        result[3] = '\0';
    } else if (next_codepoint <= 0x10FFFF) {
        // 4 bytes
        result[0] = 0b11110000 | (next_codepoint >> 18);
        result[1] = 0b10000000 | ((next_codepoint >> 12) & 0b00111111);
        result[2] = 0b10000000 | ((next_codepoint >> 6) & 0b00111111);
        result[3] = 0b10000000 | (next_codepoint & 0b00111111);
        result[4] = '\0';
    } else {
        return;  
    }
}

int main(){
    /*
    //driver code for everything before utfanalyzer

//output for function 1
printf("Is 🔥 ASCII? %d\n", is_ascii("🔥"));
//=== Output ===
//Is 🔥 ASCII? 0
printf("Is abcd ASCII? %d\n", is_ascii("abcd"));
//=== Output ===
//Is abcd ASCII? 1

//output for function 2
int32_t ret = 0;
char str[] = "abcd";
ret = capitalize_ascii(str);
printf("Capitalized String: %s\nCharacters updated: %d\n", str, ret);
//=== Output ===
//Capitalized String: ABCD
//Characters updated: 4

//output for function 3
char s[] = "Héy"; // same as { 'H', 0xC3, 0xA9, 'y', 0 },   é is start byte + 1 cont. byte
printf("Width: %d bytes\n", width_from_start_byte(s[1])); // start byte 0xC3 indicates 2-byte sequence
//=== Output ===
//Width: 2 bytes
printf("Width: %d bytes\n", width_from_start_byte(s[2])); // start byte 0xA9 is a continuation byte, not a start byte
//=== Output ===
//Width: -1

//output for function 4
char str1[] = "Joséph";
printf("Length of string %s is %d\n", str1, utf8_strlen(str1));  // 6 codepoints, (even though 7 bytes)
//=== Output ===
//Length of string Joséph is 6

//output for function 5
char str2[] = "Joséph";
int32_t idx = 4;
printf("Codepoint index %d is byte index %d\n", idx, codepoint_index_to_byte_index("Joséph", idx));
//=== Output ===
//Codepoint index 4 is byte index 5

//output for function 6
char result[17];
char string[]="🦀🦮🦮🦀🦀🦮🦮";
utf8_substring(string, 3, 7, result);
printf("String: %s\nSubstring: %s\n",string, result); // these emoji are 4 bytes long

//=== Output ===
//String: 🦀🦮🦮🦀🦀🦮🦮
//Substring: 🦀🦀🦮🦮

//output for function 7
char str3[] = "Joséph";
int32_t idx1 = 4;
printf("Codepoint at %d in %s is %d\n", idx1, str3, codepoint_at(str3, idx1)); // 'p' is the 4th codepoint
//=== Output ===
//Codepoint at 4 in Joséph is 112

//output for function 8
 char animals[] = "🐕🐇🐫💻adsfdfsasf"; // Array of emojis
    int32_t cpi = 2; // Target codepoint index (3rd emoji)
printf("Is animal: %d\n",is_animal_emoji_at(animals,cpi));
}
    
    */

   //utf analyzer driver
    //1
  char input[256];
    printf("Enter a UTF-8 encoded string: ");
    fgets(input, sizeof(input), stdin);
   
    input[strcspn(input, "\n")]='\0';
 bool validASCII=is_ascii(input);
      if(validASCII){
        printf("\nValid ASCII: true\n");
      }else{
                printf("\nValid ASCII: false\n");

      }
      //2
      char caps[256];
      strcpy(caps,input);
    int count= capitalize_ascii(caps);
      printf("Uppercased ASCII: %s\n", caps);

      //3
      int bytes=strlen(input);
      printf("Length in bytes: %d\n",bytes);
    
    //4
    int codepoints=utf8_strlen(input);
    printf("Number of code points: %d\n",codepoints);
    
      //5
          printf("Bytes per code point: ");
    for (int32_t i = 0; i < codepoints; i++) {
        int32_t bi = codepoint_index_to_byte_index(input, i);
        int32_t nextbi = codepoint_index_to_byte_index(input, i + 1);
      
        printf("%d ", nextbi - bi);
    }
    printf("\n");

 
    //6
char substring[256];
int32_t begin = 0;
int32_t end = 6;
utf8_substring(input, begin, end, substring);
printf("Substring of the first %d code points: \"%s\"\n", end, substring);


  //7
printf("Code points as decimal numbers: ");
for (int32_t i = 0; i < codepoints; i++) {
    int32_t cp = codepoint_at(input, i);
        printf("%d ", cp);
    
}
printf("\n");
     // 8. Animal emojis
    printf("Animal emojis: ");
    for (int32_t i = 0; i < codepoints; i++) {
        if (is_animal_emoji_at(input, i)) {
            int32_t bi = codepoint_index_to_byte_index(input, i);
            int32_t nextbi = codepoint_index_to_byte_index(input, i + 1);
            fwrite(input + bi, 1, nextbi - bi, stdout);
        }
    }
    printf("\n");

    char result[1000];
    int32_t idx=3;
    next_utf8_char(input,idx,result);
    printf("Next Character of Codepoint at Index %d: %s\n",idx,result);





}
