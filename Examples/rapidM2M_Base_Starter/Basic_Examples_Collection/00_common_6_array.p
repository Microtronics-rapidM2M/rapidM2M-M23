/*
 *                  _     _  __  __ ___  __  __
 *                 (_)   | ||  \/  |__ \|  \/  |
 *  _ __ __ _ _ __  _  __| || \  / |  ) | \  / |
 * | '__/ _` | '_ \| |/ _` || |\/| | / /| |\/| |
 * | | | (_| | |_) | | (_| || |  | |/ /_| |  | |
 * |_|  \__,_| .__/|_|\__,_||_|  |_|____|_|  |_|
 *           | |
 *           |_|
 * 
 * Simple "Array" Example
 * 
 * Declarations and handling of arrays and the sizeof operator
 * 
 * Compatible with every rapidM2M hardware
 *
 * @version 20190717
 */

/* Application entry point */
main()
{
  /* In PAWN there are packed and unpacked arrays. A packed array contains elements of 8 bits.
     The ASCII string "Hello" contains 5 bytes of data. So in this case two 32 bit cells are
     allocated to store 5 bytes of data. The sizeof operator always returns the amount of
     cells, which in this case is 2.
     
     ----------------------------------------------------
     "H" - 0x48 - 0 1 0 0 1 0 0 0 -> 1st byte of 1st cell
     "e" - 0x65 - 0 1 1 0 0 1 0 1 -> 2nd byte of 1st cell
     "l" - 0x6C - 0 1 1 0 1 1 0 0 -> 3rd byte of 1st cell
     "l" - 0x6C - 0 1 1 0 1 1 0 0 -> 4th byte of 1st cell
     ----------------------------------------------------
     "o" - 0x6F - 0 1 1 0 1 1 1 1 -> 1st byte of 2nd cell
     --- - ---- - x x x x x x x x -> 2nd byte of 2nd cell
     --- - ---- - x x x x x x x x -> 3rd byte of 2nd cell
     --- - ---- - x x x x x x x x -> 4th byte of 2nd cell
     ----------------------------------------------------
  */
  new message1{5} = "Hello";
  printf("1. %d\r\n", sizeof message1);
  
  /* An unpacked array contains elements of 32 bits. The ASCII characters of "Hello" are
     stored in 5 elements with 32 bits each. Thus, the sizeof operator returns 5. This
     is not recommended for characters as it wastes memory and the compiler returns warning
     messages.
     
     ----------------------
     0x48 - "H" -> 1st cell
     ----------------------
     0x65 - "e" -> 2nd cell
     ----------------------
     0x6C - "l" -> 3rd cell
     ----------------------
     0x6C - "l" -> 4th cell
     ----------------------
     0x6F - "o" -> 5th cell
     ----------------------
  */
  new message2[5] = ["H","e","l","l","o"];
  printf("2. %d\r\n", sizeof message2);
  
  /* Changes a single byte of a packed array */
  message1{1} = 'a';                        // Zero based index
  printf("3. %s\r\n", message1);            // Hello in German
  
  /* Declarations of multi-dimensional arrays */
  new MultiDimArr1[4][3] = [[100,200,300], [400,500,600], [700,800,900], [1000,1100,12000]];
  new MultiDimArr2[7]{5} = ["One", "Two", "Three", "Four", "Five", "Six", "Seven"];
  
  /* 3rd element of 1st sub-array */
  printf("4. %d\r\n",MultiDimArr1[0][2]);
  /* 1st element of 4th sub-array */
  printf("5. %d\r\n",MultiDimArr1[3][0]);
  
  /* 3rd sub-array */
  printf("6. %s\r\n",MultiDimArr2[2]);
  
  /* Changes 7th sub-array */
  MultiDimArr2[6] = "Sieben";               // Seven in German
  /* 3rd element of 7th sub-array */
  printf("7. %c\r\n",MultiDimArr2[6]{2});
  
  /* The sizeof operator can return the number of elements in each array dimension */
  printf("8. %d, %d\r\n", sizeof MultiDimArr1, sizeof MultiDimArr1[]);
  printf("9. %d, %d\r\n", sizeof MultiDimArr2, sizeof MultiDimArr2[]);
  // Note: sizeof always returns number of cells
}
