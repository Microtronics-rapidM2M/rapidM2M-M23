/**
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
 * A two-dimensional array is used to store several data records. 
 * For the first dimension a value is used to specify the number of array elements (i.e. the number of data records that can be stored).
 * For the second dimension a list of symbols is used to specify the number of array elements (i.e. the structure of a single data record). 
 * This example demonstrates two different ways to write the desired data to the two-dimensional array. In the last part of the example the 
 * data records stored in the two-dimensional array are issued via the console.
 *
 * Note: A two-dimensional array is an “array of single-dimensional arrays”. 
 * 
 * Compatible with every rapidM2M hardware
 *
 * @version 20190619  
 */

/** 
 * Structure of a data record
 *
 * Tag[]:u8		- String
 * Count:s32	- s32 Value
 * Avg:s32		- s32 Value
 */ 
#define TData[.Tag{10}, .Count, .Avg]

const
{
  DATA_NUM = 10,                            // Number of records that can be stored in the array
}

// Two-dimensional array to store the data records
// 
// For the first dimension a value (DATA_NUM = 10) is used to specify the number of array elements.
// For the second dimension a list of symbols (TData[.Tag{10}, .Count, .Avg]) is used to specify the number of array elements. 
static aDataStorage[DATA_NUM][TData];

/* application entry point */
main()
{
  // Temporary array to store a single data record. A list of symbols (TData[.Tag{10}, .Count, .Avg]) is used to specify the number of array elements.
  new aItem[TData]
  
  // Copies the desired data into the temporary array for storing a single data record. 
  aItem.Tag   = "5500011";                  // Copies the string including the terminating zero
  aItem.Count = 3;
  aItem.Avg   = 2;
  
  // Copies the temporary array for storing a single data record to the first sub-array of the two-dimensional array for storing the data records  
  // Note: In PAWN it is possible to use "=" to copy an array to another array as long as they have the same structure.   
  aDataStorage[0] = aItem;
  
  // Copies the desired data into the second sub-array of the two-dimensional array for storing the data records  
  aDataStorage[1].Tag   = "1100055";        // Copies the string including the terminating zero
  aDataStorage[1].Count = 3;
  aDataStorage[1].Avg   = 2;
  
  for( new iIdx=0; iIdx < DATA_NUM; iIdx++) // For all data records -> 
  {
    // Issues the data of the data records via the console	
    printf("%s %d %d\r\n",   aDataStorage[iIdx].Tag, aDataStorage[iIdx].Count, aDataStorage[iIdx].Avg);
  }
}
