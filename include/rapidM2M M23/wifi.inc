#pragma library wifi

/**
 * <summary>
 *   WiFi states.
 * </summary>
 */
const
{
  WIFI_STATE_OFF = 0,
  WIFI_STATE_INIT,
  WIFI_STATE_READY,
  WIFI_STATE_BUSY
};

/**
 * <summary>
 *   WiFi events.
 * </summary>
 */
const
{
  WIFI_EVENT_SCAN            = 0,    // access point found during scan
  WIFI_EVENT_DEVINFO         = 1,    // device info available
  WIFI_EVENT_CONNECTED       = 2,    // connected to WiFi network (access point)
  WIFI_EVENT_DISCONNECTED    = 3,    // disconnected from WiFi network (access point)
  WIFI_EVENT_RSSI            = 4,    // RSSI value available
  WIFI_EVENT_ERROR           = 5,    // internal error occured -> WiFi will be powered off
  
  WIFI_EVENT_SOCKET_BIND     = 100,  // feedback for socket call WiFi_bind()
  WIFI_EVENT_SOCKET_LISTEN   = 101,  // feedback for socket call WiFi_listen()
  WIFI_EVENT_SOCKET_ACCEPT   = 102,  // feedback for incoming TCP connection
  WIFI_EVENT_SOCKET_CONNECT  = 103,  // feedback for socket call WiFi_sockConnect()
  WIFI_EVENT_SOCKET_RECV     = 104,  // TCP receive
  WIFI_EVENT_SOCKET_RECVFROM = 105,  // UDP receive
  WIFI_EVENT_SOCKET_SEND     = 106,  // data was sent over socket (TCP)
  WIFI_EVENT_SOCKET_SENDTO   = 107,  // data was sent over socket (UDP)
  WIFI_EVENT_DNS_RESOLVE     = 108   // feedback for DNS resolve (corresponding IP address)
};

/**
 * <summary>
 *   Internal WiFi errors.
 * </summary>
 */
const
{
  WIFI_ERR_UNSPECIFIED = 0,       // unspecified internal error
  WIFI_ERR_INIT = 1,              // error during initialization
  WIFI_ERR_AP_SCAN = 2,           // error during access point scan
  WIFI_ERR_IO = 3                 // error IO communication
};

/**
 * <summary>
 *   WiFi error structure (WIFI_EVENT_ERROR).
 * </summary>
 * <param name="id">
 *   Error identifier, s8 (see WIFI_ERR_XXX definitions)
 * </param>
 */
#define TWiFi_Error[.id]

/**
 * <summary>
 *   WiFi devinfo structure (WIFI_EVENT_DEVINFO).
 * </summary>
 * <param name="chipname">
 *   Name of WiFi module, string
 * </param>
 * <param name="hwrev">
 *   HW revision of WiFi module, string
 * </param>
 * <param name="fwrev">
 *   FW revision of WiFi module, string
 * </param>
 * <param name="mac">
 *   MAC address of WiFi module
 * </param>
 */
#define TWiFi_DevInfo[.chipname{20+1}, .hwrev{20+1}, .fwrev{20+1}, .mac{6}]

/**
 * <summary>
 *   WiFi scan structure (WIFI_EVENT_SCAN).
 * </summary>
 * <param name="sec">
 *   security type, u8, see WIFI_SEC_xxx definitions
 * </param>
 * <param name="ch">
 *   WiFi RF channel (1-14, 2.4 GHz range), u8
 * </param>
 * <param name="bssid">
 *   Basic Service Set Identification (i.e. Access point MAC address)
 * </param>
 * <param name="ssid">
 *   Service Set Identifier
 * </param>
 * <param name="rssi">
 *   WiFi RSSI level [dBm], s8
 * </param>
 */
#define TWiFi_Scan[.sec, .ch, .bssid{6}, .ssid{32+1}, .rssi]

/**
 * <summary>
 *   WiFi network IP configuration types.
 * </summary>
 */
const
{
  WIFI_IP_CONFIG_DHCP = 1,         // IP obtained from DHCP server
  WIFI_IP_CONFIG_STATIC = 2,       // static IP
}

/**
 * <summary>
 *   Data structure for WIFI_EVENT_CONNECTED.
 * </summary>
 * <param name="sec">
 *   security type, u8, see WIFI_SEC_xxx definitions
 * </param>
 * <param name="ssid">
 *   Service Set Identifier
 * </param>
 * <param name="iptype">
 *   Type of IP adress configuration (STATIC, DHCP), u8, see WIFI_IP_CONFIG_xxx definitions
 * </param>
 * <param name="ip">
 *   own IP adress (IPv4)
 * </param>
 * <param name="sn">
 *   Subnet mask
 * </param>
 * <param name="gw">
 *   Default gateway 
 * </param>
 * <param name="dns">
 *   DNS server address 
 * </param>
 */
#define TWiFi_Connected[.sec, .ssid{32+1}, .iptype, .ip{4}, .sn{4}, .gw{4}, .dns{4}]

/**
 * <summary>
 *   Causes for disconnect from WiFi network (access point).
 * </summary>
 */
const
{
  WIFI_DISCONNECT_OK = 0,                   // regular disconnection

  WIFI_DISCONNECT_SCAN_FAIL       = -1,     // scan failed
  WIFI_DISCONNECT_JOIN_FAIL       = -2,     // failed to join the BSS
  WIFI_DISCONNECT_AUTH_FAIL       = -3,     // failed to authenticate with the AP
  WIFI_DISCONNECT_ASSOC_FAIL      = -4,     // failed to associate with the AP
  WIFI_DISCONNECT_CONN_INPROGRESS = -5,     // another connection request in progress
  WIFI_DISCONNECT_INTERNAL_ERR    = -6,     // internal driver error

  WIFI_DISCONNECT_SETTINGS        = -100,   // probably wrong settings (e.g. key too long/short)
  WIFI_DISCONNECT_CONNECT_FAIL    = -101,   // probably AP not available or wrong settings
  WIFI_DISCONNECT_AP_NOT_FOUND    = -102,   // AP (SSID) not found during scan
  WIFI_DISCONNECT_AP_SECURITY     = -103,   // AP security not supported
  WIFI_DISCONNECT_WEP_PWDLEN      = -104,   // PWD length for WEP is not valid
}

/**
 * <summary>
 *   Data structure for WIFI_EVENT_DISCONNECTED.
 * </summary>
 * <param name="cause">
 *  Cause for disconnection, see WIFI_DISCONNECT_xxx definitions
 * </param>
 */
#define TWiFi_Disconnected[.cause]

/**
 * <summary>
 *   Data structure for WIFI_EVENT_RSSI.
 * </summary>
 * <param name="rssi">
 *  WiFi RSSI level [dBm], s8 
 * </param>
 */
#define TWiFi_Rssi[.rssi]

/**
 * <summary>
 *   WiFi security types for access point authentication.
 * </summary>
 */
const
{
  WIFI_SEC_INVALID = 0, // Invalid security type
  WIFI_SEC_OPEN,        // WiFi network is not secured
  WIFI_SEC_WPA_PSK,     // WiFi network is secured with WPA/WPA2 personal (PSK)
  WIFI_SEC_WEP,         // Security type WEP (40 or 104) OPEN OR SHARED
  WIFI_SEC_802_1X       // WiFi network is secured with WPA/WPA2 Enterprise
                        // IEEE802.1x user-name/password authentication
};

/**
 * <summary>
 *   Init and configure WiFi interface.
 * </summary>
 *
 * <param name="funcidx">
 *   Index of event callback function. Function is called on WiFi events.
 *   Has to be of type: public func(event, connhandle, const data{}, len);
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native WiFi_Init(funcidx);

/**
 * <summary>
 *   Close and deactivate WiFi interface.
 * </summary>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native WiFi_Close();

/**
 * <summary>
 *   Get WiFi interface state.
 * </summary>
 *
 * <returns>
 *   State of WiFi interface (see WiFi states).
 * </returns>
 */
native WiFi_GetState();

/**
 * <summary>
 *   Start WiFi scan on all channels.
 * </summary>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native WiFi_Scan();

/**
 * <summary>
 *   Connect to access point.
 * </summary>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native WiFi_Connect();

/**
 * <summary>
 *   Disconnect from access point.
 * </summary>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native WiFi_Disconnect();

/**
 * <summary>
 *   Get current WiFi RSSI level (device must be connected with AP).
 * </summary>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native WiFi_GetRSSI();

/**
 * <summary>
 *   Access point configuration
 * </summary>
 * <param name="sec">
 *   security type, see WIFI_SEC_xxx definitions
 * </param>
 * <param name="ssid">
 *   Service Set Identifier
 * </param>
 * <param name="ssidHide">
 *   0 = SSID visible, 1 = SSID hidden
 * </param>
 * <param name="pwd">
 *   password for authentication (depending on security type)
 *   note: WIFI_SEC_WPA_PSK requires at least 9 characters !
 * </param>
 * <param name="rfchnl">
 *   RF channel for access point operation
 * </param>
 * <param name="ip">
 *   Access point IP adress (IPv4)
 * </param>
 */
#define TWiFi_APCfg[.sec, .ssid{32+1}, .ssidHide, .pwd{64+1}, .rfchnl, .ip[4]]

/**
 * <summary>
 *   Configure and enable access point.
 * </summary>
 *
 * <param name="cfg">
 *   access point configuration
 * </param>
 * <param name="len">
 *   length of provided configuration (number of cells)
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native WiFi_APInit(cfg[TWiFi_APCfg], len=sizeof cfg);

/**
 * <summary>
 *  Address family definitions used for socket handling
 * </summary>
 */
const
{
  WIFI_AF_INET = 2       /** address family used for IPv4 */
};

/**
 * <summary>
 *  Socket type definitions
 * </summary>
 */
const
{
  WIFI_SOCK_STREAM = 1,     /** reliable connection-oriented stream connection, i.e. TCP */
  WIFI_SOCK_DGRAM  = 2      /** unreliable connectionless datagram connection, i.e. UDP */
};

/**
 * <summary>
 *   Socket address structure for IPV4 addresses.
 *   Used to specify socket address infomation to which to connect to
 * </summary>
 * <param name="sin_family">
 *   adress family, must be 'WIFI_AF_INET' for IPv4
 * </param>
 * <param name="sin_port">
 *   Socket port number
 * </param>
 * <param name="sin_addr">
 *   Socket IP address
 *   Can be set to "0" to accept any IP address for server operation
 * </param>
 */
#define Tsockaddr_in [.sin_family, .sin_port, .sin_addr[4]]

/**
 * <summary>
 *   Errorcodes for socket operations.
 * </summary>
 */
const
{
  /* Socket address is invalid. The socket operation cannot be completed successfully
   * without specifying a specific address 
   * For example: bind is called without specifying a port number */
  WIFI_SOCK_ERR_INVALID_ADDRESS	= -11,

  /* Socket operation cannot bind on the given address. With socket operations, only
   * one IP address per socket is permitted. Any attempt for a new socket to bind with
   * an IP address already bound to another open socket, will return the following
   * error code. States that bind operation failed. */
  WIFI_SOCK_ERR_ADDR_IN_USE = -12,

  /* Exceeded the maximum number of TCP sockets */
  WIFI_SOCK_ERR_MAX_TCP_SOCK = -13,
  
  /* Exceeded the maximum number of UDP sockets */
  WIFI_SOCK_ERR_MAX_UDP_SOCK = -14,

  /* NOTE: SOCK_ERR -15 intentionally omitted ! */

  /* An invalid argument is passed to a function. */
  WIFI_SOCK_ERR_INVALID_ARG = -16,

  /* Exceeded the maximum number of TCP passive listening sockets */
  WIFI_SOCK_ERR_MAX_LISTEN_SOCK = -17,

  /* NOTE: SOCK_ERR -18 intentionally omitted ! */

  /* The requested socket operation is not valid in the current socket state. */
  WIFI_SOCK_ERR_INVALID = -19,
  
  /* NOTE: SOCK_ERR -20 intentionally omitted ! */
  
  /* Destination address is required. */
  WIFI_SOCK_ERR_ADDR_IS_REQUIRED = -21,

  /* The socket is closed by the peer. The local socket is closed also */
  WIFI_SOCK_ERR_CONN_ABORTED = -22,

  /* The socket pending operation has Timedout */
  WIFI_SOCK_ERR_TIMEOUT = -23,

  /* No buffer space available to be used for the requested socket operation. */
  WIFI_SOCK_ERR_BUFFER_FULL = -24,

  /* not enough space within recv buffer */
  WIFI_SOCK_ERR_RECV_BUF_FULL = -100,

  /* memory context not available -> operation not possible */
  WIFI_SOCK_ERR_NO_MEMCTX = -101,
};

/**
 * <summary>
 *   Create a socket.
 *   On success, socket is immediately ready to use (synchronous operation).
 * </summary>
 *
 * <param name="domain">
 *   protocol family (WIFI_AF_INET)
 * </param>
 * <param name="type">
 *   type of socket ('WIFI_SOCK_STREAM' for TCP, 'WIFI_SOCK_DGRAM' for UDP)
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>socket handle (non-negative value) if successful.</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_socket(domain, type, flags=0);

/**
 * <summary>
 *   Data structure for WIFI_EVENT_SOCKET_BIND.
 * </summary>
 * <param name="result">
 *   Resultcode for BIND operation, s8.
 *   0=successful, negative value: error (see WIFI_SOCK_ERR_xxx definitions)
 * </param>
 */
#define TWiFi_Bind[.result]

/**
 * <summary>
 *   bind socket - assign address and port, asynchronous.
 *   Asynchronous response within 'WIFI_EVENT_SOCKET_BIND' callback.
 * </summary>
 *
 * <param name="socket">
 *   socket for bind operation. Must be created with WiFi_socket() previously.
 * </param>
 * <param name="sockaddr">
 *   socket address information (see Tsockaddr_in from socket.h)
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>zero if successful, BIND operation started.</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_bind(socket, sockaddr[Tsockaddr_in]);

/**
 * <summary>
 *   Data structure for WIFI_EVENT_SOCKET_LISTEN.
 * </summary>
 * <param name="result">
 *   Resultcode for LISTEN operation, s8.
 *   0=successful, negative value: error (see WIFI_SOCK_ERR_xxx definitions)
 * </param>
 */
#define TWiFi_Listen[.result]

/**
 * <summary>
 *   Data structure for WIFI_EVENT_SOCKET_ACCEPT.
 * </summary>
 * <param name="socket">
 *   Remote socket, s8.
 *   negative value: error (see WIFI_SOCK_ERR_xxx definitions)
 * </param>
 * <param name="family">
 *   address family for remote socket (WIFI_AF_INET), u16.
 * </param>
 * <param name="port">
 *   remote port, u16.
 * </param>
 * <param name="addr">
 *   remote IPv4 address
 * </param>
 */
#define TWiFi_Accept[.socket, .family, .port, .addr{4}]

/**
 * <summary>
 *   listen to incoming connections (TCP server). WiFi_bind() is required before.
 *   Asynchronous operation, response within 'WIFI_EVENT_SOCKET_LISTEN'.
 *   If response is successful, socket is ready for incoming connections. Incoming
 *   connections will trigger event 'WIFI_EVENT_SOCKET_ACCEPT'.
 * </summary>
 *
 * <param name="socket">
 *   socket for listen operation.
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>zero if successful, LISTEN operation started.</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_listen(socket);

/**
 * <summary>
 *   Data structure for WIFI_EVENT_SOCKET_CONNECT.
 * </summary>
 * <param name="result">
 *   Resultcode for CONNECT operation, s8.
 *   0=successful, negative value: error (see WIFI_SOCK_ERR_xxx definitions)
 * </param>
 */
#define TWiFi_SockConnect[.result]

/**
 * <summary>
 *   Establish a TCP connection with a remote server.
 *   Asynchronous response within 'WIFI_EVENT_SOCKET_CONNECT' callback.
 * </summary>
 *
 * <param name="socket">
 *   socket for connect operation. Must be created with WiFi_socket() previously.
 * </param>
 * <param name="remoteaddr">
 *   remote address information (see Tsockaddr_in from socket.h)
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>zero if successful, CONNECT operation started.</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_sockConnect(socket, remoteaddr[Tsockaddr_in]);

/**
 * <summary>
 *   Data structure for WIFI_EVENT_SOCKET_RECV
 * </summary>
 * <param name="received">
 *   number of bytes received, s16.
 *   positive number if successful, zero if socket closed, negative if error (see WIFI_SOCK_ERR_xxx definitions)
 *   Note: socket must be closed using 'WiFi_sockClose' function if error occured !
 * </param>
 */
#define TWiFi_Recv[.received]

/**
 * <summary>
 *   Asynchronous function for receiving data on connected socket (TCP)
 *   Event WIFI_EVENT_SOCKET_RECV is triggered for processing
 * </summary>
 *
 * <param name="socket">
 *   socket for recv operation.
 * </param>
 * <param name="buf">
 *   data buffer for recv operation (public).
 * </param>
 * <param name="len">
 *   length of data buffer (number of bytes).
 * </param>
 * <param name="timeout">
 *   timeout for data receive [ms], 0=untimed
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>zero if successful</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_recv(socket, buf{}, len, timeout=0);

/**
 * <summary>
 *   Data structure for WIFI_EVENT_SOCKET_RECVFROM
 * </summary>
 * <param name="received">
 *   number of bytes received, s16.
 *   positive number if successful, zero if socket closed, negative if error (see WIFI_SOCK_ERR_xxx definitions)
 *   Note: socket must be closed using 'WiFi_sockClose' function if error occured !
 * </param>
 * <param name="family">
 *   address family for remote socket (WIFI_AF_INET), u16.
 * </param>
 * <param name="port">
 *   remote port, u16.
 * </param>
 * <param name="addr">
 *   remote IPv4 address
 * </param>
 */
#define TWiFi_Recvfrom[.received, .family, .port, .addr{4}]

/**
 * <summary>
 *   Asynchronous function for receiving data (UDP)
 *   Event WIFI_EVENT_SOCKET_RECVFROM is triggered for processing
 * </summary>
 *
 * <param name="socket">
 *   socket for recvfrom operation.
 * </param>
 * <param name="buf">
 *   data buffer for recvfrom operation (public).
 * </param>
 * <param name="len">
 *   length of data buffer (number of bytes).
 * </param>
 * <param name="timeout">
 *   timeout for data receive [ms], 0=untimed
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>zero if successful.</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_recvfrom(socket, buf{}, len, timeout=0);

/**
 * <summary>
 *   Set memory context for socket.
 *   This memory is used internally for processing data within socket.
 *   Provided memory must be public. Accessing this memory from script is not meaningful.
 *   This function should be called immediately after creating socket.
 * </summary>
 *
 * <param name="socket">
 *   selected socket.
 * </param>
 * <param name="rxbuf">
 *   used for buffering rx data.
 * </param>
 * <param name="rxbuflen">
 *   length of rx buffer (number of bytes).
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li><em>OK</em> if successful.</li>
 *     <li><em>ERROR</em> - if any error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_sockMemCtx(socket, rxbuf{}, rxbuflen);

/**
 * <summary>
 *   Data structure for WIFI_EVENT_SOCKET_SEND (TCP) and WIFI_EVENT_SOCKET_SENDTO (UDP).
 * </summary>
 * <param name="result">
 *   number of sent bytes, s16.
 *   positive number if successful, negative number if error (see WIFI_SOCK_ERR_xxx definitions)
 * </param>
 */
#define TWiFi_Send[.sent]

/**
 * <summary>
 *   Asynchronous sending function, used to send data over a TCP socket.
 *   Event 'WIFI_EVENT_SOCKET_SEND' is fired if data was sent
 * </summary>
 *
 * <param name="socket">
 *   socket for send operation.
 * </param>
 * <param name="buf">
 *   data buffer for send operation.
 *   Note: data buffer must be public and will be modified by the called function !
 * </param>
 * <param name="len">
 *   number of bytes to send, max=1400.
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>number of consumed bytes if successful.</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_send(socket, buf{}, len);

/**
 * <summary>
 *   Asynchronous sending function, used to send data over a UDP socket.
 *   Event 'WIFI_EVENT_SOCKET_SENDTO' is fired if data was sent
 * </summary>
 *
 * <param name="socket">
 *   socket for send operation.
 * </param>
 * <param name="buf">
 *   data buffer for send operation.
 *   Note: data buffer must be public and will be modified by the called function !
 * </param>
 * <param name="len">
 *   number of bytes to send, max=1400.
 * </param>
 * <param name="remoteaddr">
 *   remote address information (see Tsockaddr_in from socket.h)
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>number of consumed bytes if successful.</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_sendto(socket, buf{}, len, remoteaddr[Tsockaddr_in]);

/**
 * <summary>
 *   Synchronous call to close socket and free all socket assigned resources.
 *   Note: All pending messages (sent or received) will be discarded !
 * </summary>
 *
 * <param name="socket">
 *   socket to close.
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>zero if successful</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_sockClose(socket);

/**
 * <summary>
 *   Data structure for WIFI_EVENT_DNS_RESOLVE.
 * </summary>
 * <param name="ip">
 *   resolved IPv4 address, u8
 *   0.0.0.0 in case of error
 * </param>
 */
#define TWiFi_DNSResolve[.ip{4}]

/**
 * <summary>
 *   Asynchronous function to resolve a host name (e.g. URL) to a host IP address
 *   via the Domain Name System (DNS). Operation depends on having configured a
 *   DNS server IP address and having access to the DNS hierarchy through the internet.
 *   Event 'WIFI_EVENT_DNS_RESOLVE' is fired if resolve process has finished (ok or nok).
 * </summary>
 *
 * <param name="hostname">
 *   hostname to be resolved.
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>zero if successful</li>
 *     <li>negative value if any error occured (see WIFI_SOCK_ERR_xxx for dedicated socket errorcodes)</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native WiFi_gethostbyname(const hostname{});
