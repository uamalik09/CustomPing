# CustomPing

The provided code is a C++ program for implementing a basic ping utility using ICMP (Internet Control Message Protocol). The program sends ICMP Echo Request packets to a specified host and measures the round-trip time (RTT) for the packets to return. Below is a breakdown of the major components of the code:

1. **Header Files:** The code includes several header files, including `<iostream>`, `<cstdlib>`, `<cstring>`, and various network-related headers such as `<netinet/ip_icmp.h>`, `<netinet/in.h>`, `<arpa/inet.h>`, `<netdb.h>`, `<sys/types.h>`, `<sys/socket.h>`, `<unistd.h>`, `<fcntl.h>`, `<signal.h>`, and others. These headers provide the necessary functions and data structures for network communication and timing.

      `<iostream>`:
      This header is from the C++ Standard Library.
      It provides input and output stream classes like cin and cout.
      In the code, it is used for printing messages to the console.

      `<cstdlib>`:
      This header is from the C Standard Library.
      It provides various functions and macros for general-purpose programming.
      In the code, it's used for functions like exit, malloc, and atoi.

      `<cstring>`:
      This header is from the C Standard Library.
      It provides functions for handling C-style strings.
      In the code, it's used for string manipulation functions like strcpy.

      `<netinet/ip_icmp.h>`:
      This header provides structures and constants for dealing with ICMP (Internet Control Message Protocol) packets.
      It's used in the code for ICMP-related data structures.

      `<netinet/in.h>`:
      This header is used for socket-related data structures and constants.
      It includes definitions for data structures like struct sockaddr_in (used for IP addresses) and constants like AF_INET (for IPv4).

      `<arpa/inet.h>`:
      This header provides functions and macros for handling IP addresses.
      It includes functions like inet_ntoa (converts IP addresses to strings) and htons (converts host byte order to network byte order).

      `<netdb.h>`:
      This header is for domain name resolution.
      It provides functions like gethostbyname for resolving hostnames to IP addresses.
      It's used in the code for DNS lookup.

      `<sys/types.h>`:
      This header defines various data types used in system calls.
      It's used to define types like socklen_t.

      `<sys/socket.h>`:      
      This header is used for socket programming and provides socket-related functions and constants.
      It includes socket types (e.g., SOCK_RAW for raw sockets) and system calls for socket operations.

      `<unistd.h>`:      
      This header is part of the Unix standard library.
      It provides access to various POSIX operating system services.
      In the code, it's used for functions like usleep (microsecond sleep).

      `<fcntl.h>`:      
      This header is used for file control options.
      In the code, it is not explicitly used, but it may be used for setting file descriptor options.

      `<signal.h>`:      
      This header deals with signal handling in C and C++.
      It provides functions and macros for registering signal handlers.
      In the code, it's used to handle Ctrl+C (SIGINT) by defining the intHandler function

      `<ctime>`:      
      This header is used for date and time functions.
      It provides functions for measuring time, setting time delays, and time-related data types like struct timespec.
      In the code, it's used to measure and record timestamps.

      `<sys/time.h>`:      
      This header is used for time-related functions and data structures.
      It includes the struct timeval and functions for setting timeouts.
      In the code, it's used for setting socket timeout values.

      Each of these headers plays a crucial role in the code, enabling various functionalities required for network programming, ICMP packet handling, signal handling, and time measurement.

2. **Preprocessor Directives:** There are several preprocessor directives that define constants and macros used in the program. For example, PING_PKT_S specifies the size of the ICMP packet, PORT_NO is set to 0, PING_SLEEP_RATE sets the delay between sending packets, and RECV_TIMEOUT sets the timeout for receiving responses.
      
      
      `#define PING_PKT_S 64`:
      
      This directive defines a constant named PING_PKT_S and sets its value to 64.
      It specifies the size of the ICMP (Internet Control Message Protocol) packet in bytes. ICMP packets are typically 64 bytes, but this size can be adjusted based on specific requirements.
      
      `#define PORT_NO 0`:
      This directive defines a constant named PORT_NO and sets its value to 0.
      In this context, setting the port number to 0 indicates that the ICMP packets are not directed at a specific port on the destination host. ICMP operates at the IP layer, and unlike protocols like TCP or UDP, it doesn't have a notion of ports.
      
      `#define PING_SLEEP_RATE 1000000`:
      This directive defines a constant named PING_SLEEP_RATE and sets its value to 1000000.
      It specifies the delay (in microseconds) between sending ICMP packets. In this case, it's set to 1 second (1000000 microseconds), which means that the program waits for one second before sending the next ICMP packet.
      
      `#define RECV_TIMEOUT 1`:
      This directive defines a constant named RECV_TIMEOUT and sets its value to 1.
      It determines the timeout (in seconds) for receiving responses to the sent ICMP packets. If a response is not received within this timeout, the program considers it a timeout.
      
      
      These preprocessor directives play a crucial role in making the code configurable and easy to adapt for different scenarios. For example, changing PING_SLEEP_RATE allows you to control the rate at which ICMP packets are sent, and modifying PING_PKT_S lets you adjust the packet size to meet specific requirements. The PORT_NO directive is set to 0 because ICMP packets do not use ports in the same way as TCP or UDP packets do. Finally, RECV_TIMEOUT determines how long the program waits for responses before considering them as timeouts, allowing you to configure the timeout behavior.

3. **Global Variables:** The code defines a global variable `pingloop` to control the main loop of the program. It is set to 1 initially and can be changed to 0 to stop the loop.

    Global Variables:

      In C++ (and C), a global variable is a variable declared outside of any function, making it accessible and modifiable from any part of the code. In this code,pingloop is declared as a global variable.
      int pingloop = 1;:
      
      This line declares a global integer variable named pingloop and initializes it with a value of 1.
      pingloop is a flag variable used to control the main loop of the program. It plays a pivotal role in determining whether the program should continue sending ICMP packets or stop.
      Usage:
      
      pingloop is used in a loop within the send_ping function. While pingloop is equal to 1, the loop continues to send ICMP packets and listen for responses. If pingloop is changed to 0, the loop terminates, effectively stopping the program.
      Control Flow:
      
      Initially setting pingloop to 1 ensures that the program's main loop begins execution.
      Inside the main loop of the program, the pingloop variable can be set to 0 when the program receives a Ctrl+C (SIGINT) signal. The intHandler function is responsible for setting pingloop to 0 when the user wants to stop the program gracefully.
   
      Graceful Termination:   
      The use of pingloop and the signal handling approach provides a way to gracefully terminate the program. When the user interrupts the program (e.g., by pressing Ctrl+C), it sets pingloop to 0, and the program finishes its current loop iteration and exits.
      In summary, pingloop is a global flag variable used to control the execution of the main loop in the program. It allows the program to continue sending ICMP packets and listening for responses until it is explicitly set to 0, typically when the user decides to stop the program. This mechanism ensures that the program can be terminated gracefully without abruptly stopping in the middle of its operation.


4. Certainly, let's delve into the data structure defined in the code, specifically the ping_pkt structure:

    `Data Structures:`

    Data structures in programming are used to organize and store data in a way that makes it accessible and manageable. In the code, a custom data structure called ping_pkt is defined to represent ICMP packets.
   
    `struct ping_pkt:` 
    This line introduces the declaration of the ping_pkt structure, which is a user-defined composite data type.
    The struct keyword is used to define a new structure named ping_pkt.
   
   `Structure Components:`
    The ping_pkt structure consists of two main components:
    struct icmphdr hdr: This is a member of the ping_pkt structure and represents the ICMP header. The ICMP header contains information about the type and code of the ICMP packet, as well as additional fields.
    char msg[PING_PKT_S - sizeof(struct icmphdr)]: This is a character array (C-style string) named msg. It represents the message or payload of the ICMP packet. The size of this array is determined by subtracting the size of the ICMP header from the value specified by the PING_PKT_S constant.
   
    `ICMP Header:` 
    The struct icmphdr hdr part of the ping_pkt structure is used to store the ICMP header information for each ICMP packet. This header includes fields such as the type of ICMP packet (e.g., ICMP Echo Request or ICMP Echo Reply) and a checksum, among others.
   
   `Message Buffer:`
    The char msg[PING_PKT_S - sizeof(struct icmphdr)] part of the ping_pkt structure is used to store the actual data or message that will be sent within the ICMP packet. It is represented as an array of characters and can hold various types of data or messages.
   
    `Purpose:`
    The ping_pkt structure is used to create instances of ICMP packets that are sent to a remote host. Each packet will contain both the ICMP header and a message. The structure allows for easy construction and manipulation of these packets.
    In summary, the ping_pkt structure is designed to represent ICMP packets in the program. It encapsulates the necessary information for both the ICMP header and the message payload within each packet. This structure simplifies the construction and handling of ICMP packets, which are essential for the functionality of the ICMP ping utility implemented in the code.


  5. The various functions defined in the code:
    
      `unsigned short checksum(void* b, int len):`
      Purpose: This function is responsible for calculating the checksum for an ICMP packet.
      Parameters:
      void* b: A pointer to the buffer containing the data for which the checksum needs to be calculated.
      int len: The length of the data buffer.
      Return Type: unsigned short
      Explanation: The checksum function takes the data buffer and its length as input and performs the checksum calculation. It is commonly used to ensure data integrity during transmission.
     
     ` void intHandler(int dummy):`
      Purpose: This function serves as an interrupt handler. It is responsible for setting the global variable pingloop to 0 when the program receives a termination signal (e.g., Ctrl+C).
      Parameters:
      int dummy: This parameter is not used within the function but is required to match the function signature expected for an interrupt handler.
      Return Type: void
      Explanation: When a termination signal (Ctrl+C) is received, the intHandler function is called. It sets pingloop to 0, which is a signal to the program's main loop to stop sending ICMP packets and exit gracefully.
     
      `char* dns_lookup(const char* addr_host, struct sockaddr_in* addr_con):` 
      Purpose: This function performs a DNS lookup to resolve a hostname to an IP address using the gethostbyname function.
      Parameters:
      const char* addr_host: The hostname that needs to be resolved to an IP address.
      struct sockaddr_in* addr_con: A pointer to a sockaddr_in structure where the resolved IP address and other connection-related information will be stored.
      Return Type: char*
      Explanation: The function uses the gethostbyname function to perform the DNS lookup. It retrieves the IP address corresponding to the provided hostname and populates the sockaddr_in structure with the necessary connection details. The resolved IP address is returned as a string.
     
      `char* reverse_dns_lookup(const char* ip_addr):`
      Purpose: This function performs a reverse DNS lookup to get the hostname associated with an IP address.
      Parameters:
      const char* ip_addr: The IP address for which the reverse DNS lookup is to be performed.
      Return Type: char*
      Explanation: Given an IP address, the function uses the getnameinfo function to perform a reverse DNS lookup. It retrieves the hostname associated with the provided IP address and returns it as a string.
     
      `void send_ping(int ping_sockfd, struct sockaddr_in* ping_addr, const char* ping_dom, const char* ping_ip, const char* rev_host):`
      Purpose: This is the core function of the program. It sends ICMP Echo Request packets, measures the round-trip time (RTT), and prints the results.
      Parameters:
      int ping_sockfd: The socket file descriptor used for sending ICMP packets.
      struct sockaddr_in* ping_addr: A pointer to a sockaddr_in structure containing the destination IP address and other connection details.
      const char* ping_dom: The hostname being pinged.
      const char* ping_ip: The IP address of the destination.
      const char* rev_host: The hostname associated with the destination IP address (reverse DNS lookup result).
      `Return Type: void`
      Explanation: This function is where the actual ping operation occurs. It constructs ICMP packets, sends them to the destination, waits for responses, and measures the RTT. The results are printed to the console, including information about packet loss, RTT, and other details.
      These functions play essential roles in the ICMP ping utility implemented in the code. They handle various aspects of packet construction, DNS resolution, reverse DNS lookup, and signal handling, making the program function as a basic ping tool.


  6. The main function in the code, which is the entry point of the program:

        `int main(int argc, char* argv[]):`
        This is the main function of the program, where execution begins.
       
        `Parameters:`
        int argc: An integer parameter representing the number of command-line arguments.
        char* argv[]: An array of character pointers that holds the command-line arguments.
       
        `Function Purpose:`
        The main function serves as the entry point of the program, and its purpose is to perform the following key tasks:
        Parse a command-line argument to specify the target host to ping.
        Perform DNS lookup to resolve the specified hostname to an IP address.
        Create a socket for sending ICMP (ping) packets.
        Set up an interrupt handler to handle termination signals gracefully.
        Call the send_ping function to send ICMP packets to the target host and measure the round-trip time (RTT).
       
        `if (argc != 2):` 
        This conditional statement checks whether the number of command-line arguments is not equal to 2. In other words, it checks if the user did not provide the required command-line argument specifying the target host to ping.
       
        `std::cout << "\nFormat " << argv[0] << " <address>" << std::endl;:`
        If the number of arguments is not 2, the program prints a message to the console, informing the user of the correct format for running the program. It includes the program name (retrieved from argv[0]) and the expected format.
       
        `char* ip_addr;:`
        This line declares a character pointer named ip_addr. It will be used to store the resolved IP address of the target host.
       
        `char* reverse_hostname;:`
        This line declares another character pointer named reverse_hostname. It will be used to store the result of the reverse DNS lookup, which is the hostname associated with the target IP address.
       
        `struct sockaddr_in addr_con;:`
        This line declares a sockaddr_in structure named addr_con. It is used to store connection-related information, including the resolved IP address and port information.
       
        `int addrlen = sizeof(addr_con);:`
        This line calculates and assigns the size of the addr_con structure to the integer variable addrlen.
       
        `DNS Lookup and Error Handling:`
        The following code block performs DNS lookup to resolve the provided hostname to an IP address. It calls the dns_lookup function and stores the result in the ip_addr variable. If the lookup fails, it prints an error message and exits the program.
       
        `Reverse DNS Lookup:`
        The program then performs a reverse DNS lookup using the reverse_dns_lookup function to get the hostname associated with the target IP address. The result is stored in the reverse_hostname variable.
       
        `Socket Creation and Error Handling:`
        The code creates a socket for sending ICMP (ping) packets using the socket function. The socket file descriptor is stored in the sockfd variable. If the socket creation fails, the program prints an error message and exits.
       
        `Signal Handling:`
        The program sets up a signal handler using the signal function to handle termination signals (e.g., Ctrl+C). The intHandler function is used for this purpose. When a termination signal is received, the handler sets pingloop to 0, indicating that the program should gracefully exit.
       
        `Calling send_ping Function:`
        Finally, the main function calls the send_ping function, passing the necessary arguments. The send_ping function is responsible for sending ICMP packets to the target host, measuring RTT, and printing the results to the console.
       
        In summary, the main function serves as the entry point of the program and orchestrates the entire process of sending ICMP packets to the target host, measuring RTT, and handling DNS-related operations. It ensures that the program gracefully exits upon receiving termination signals.

# Usage
 1. compile the code ` g++ ping.cpp -o ping`
 2. run ` sudo ./ping <target> `

![output](./example.png?raw=true "output")
