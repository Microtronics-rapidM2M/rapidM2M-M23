#pragma library nvram

/**
 * <summary>
 *   NVRAM Information structure.
 * </summary>
 *
 * <param name="size">
 *   Number of available bytes.
 * </param>
 */
#define TNVRAM_Info[.size]

/**
 * <summary>
 *   Get NVRAM information.
 * </summary>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>number of available cells if successful.</li>
 *     <li><em>ERROR_NOT_SUPPORTED</em> - if NVRAM is not available on this hardware.</li>
 *     <li><em>ERROR</em> - if any other error detected.</li>
 *   </ul>
 * </returns>
 */
native NVRAM_GetInfo(info[TNVRAM_Info], len=sizeof info)

/**
 * <summary>
 *   Read data from NVRAM.
 * </summary>
 *
 * <param name="addr">
 *   Start address within NVRAM.
 * </param>
 * <param name="data">
 *   Array of data for storing the read bytes.
 * </param>
 * <param name="len">
 *   Number of bytes to read.
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li><em>OK</em> - if successful.</li>
 *     <li><em>ERROR_NOT_SUPPORTED</em> - if NVRAM is not available on this hardware.</li>
 *     <li><em>ERROR</em> - if any other error detected.</li>
 *   </ul>
 * </returns>
 */
native NVRAM_Read(addr, const data{}, len)

/**
 * <summary>
 *   Write data to NVRAM.
 * </summary>
 *
 * <param name="addr">
 *   Start address within NVRAM.
 * </param>
 * <param name="data">
 *   Array of data to write.
 * </param>
 * <param name="len">
 *   Number of bytes to write.
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li><em>OK</em> - if successful.</li>
 *     <li><em>ERROR_NOT_SUPPORTED</em> - if NVRAM is not available on this hardware.</li>
 *     <li><em>ERROR</em> - if any other error detected.</li>
 *   </ul>
 * </returns>
 */
native NVRAM_Write(addr, const data{}, len)

