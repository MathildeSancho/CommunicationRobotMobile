
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "gopigo.h"

int main(int argc, char **argv)
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    int s, client, bytes_read;
    socklen_t opt = sizeof(rem_addr);

    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));


    if(init()==-1)
      exit(1);
    led_on(1);
    led_on(0);

    while(1){
      // put socket into listening mode
      listen(s, 1);
      printf("listen mode");

      // accept one connection
      client = accept(s, (struct sockaddr *)&rem_addr, &opt);

      ba2str( &rem_addr.rc_bdaddr, buf );
      fprintf(stderr, "accepted connection from %s\n", buf);

      while(1){
	memset(buf, 0, sizeof(buf));
	// read data from the client
	bytes_read = read(client, buf, sizeof(buf));
    
	if( bytes_read > 0 ) {
	  printf("received [%s]\n", buf);
	  
	  switch (buf[0])
	    {
	    case 'w': case 'W':
	      fwd();
	      break;

	    case 'a': case 'A':
	      left();
	      break;

	    case 'd': case 'D':
	      right();
	      break;

	    case 's': case 'S':
	      bwd();
	      break;

	    case 'e': case 'E':
	      increase_speed();
	      break;

	    case 'g': case 'G':
	      decrease_speed();
	      break;

	    case 'z': case 'Z':
	      exit(0);
	      break;

	    case 'x': case 'X':
	      stop();
	      break;

	    case 'i': case 'I':
	      motor_fwd();
	      break;

	    case 'k': case 'K':
	      motor_bwd();
	      break;

	    case 'n': case 'N':
	      left_rot();
	      break;

	    case 'm': case 'M':
	      right_rot();
	      break;

	    case 'l': case 'L':
	      switch(buf[1])
		{
		case 'o': case 'O':
		  ;
		  int led_val;
		  printf("Enter 1 for left led and 0 for right led:\n");
		  scanf(" %d", &led_val);
		  led_on(led_val);
		  break;
		case 'f': case 'F':
		  ;
		  int led_val2;
		  printf("Enter 1 for left led and 0 for right led:\n");
		  scanf(" %d", &led_val2);
		  led_off(led_val2);
		  break;
		default:
		  printf("invalid led command... (o for ON, f for OFF)\n");
		  break;
		}
	      break;

	    case 'r': case 'R':
	      ;
	      int servo_val;
	      printf("Enter servo position:\n");
	      scanf(" %d", &servo_val);
	      servo(servo_val);
	      break;

	    case 'v': case 'V':
	      printf("Firmware Version: %d\n",fw_ver());
	      break; 
                
	    case 'u': case 'U':
	      printf("Ultrasonic Dist: %d\n",us_dist(15));
	      break;
                    
	    case 't': case 'T':
	      switch(buf[1])
		{
		case 'r': case 'R':
		  ;
		  int val = trim_read();
		  if(val == -3){
		    printf("-3, Trim Value Not set\n");
		  }
		  else{
		    printf("%d\n", val-100);
		  }
		  break;
		case 'w': case 'W':
		  ;
		  int val2;
		  printf("Enter trim value to write to EEPROM(-100 to 100):\n");
		  scanf(" %d", &val2);
		  trim_write(val2);
		  pi_sleep(100);
		  printf("Value in EEPROM: %d\n", trim_read()-100);
		  break;
		case 't': case 'T':
		  ;
		  int val3;
		  printf("Enter trim value to test(-100 to 100):\n");
		  scanf(" %d", &val3);
		  trim_test(val3);
		  pi_sleep(100);
		  printf("Value in EEPROM: %d\n", trim_read()-100);
		  break;
		default:
		  printf("invalid trim command...\n");
		  break;
		}
	      break;

	    case 'c': case 'C':
	      printf("Motor 0 (left) travelled %i cm\n", enc_read(0));
	      printf("Motor 1 (right) travelled %i cm\n", enc_read(1));
	      break;

	    default:
	      printf("invalid command...\n");
	      break;
	    }
	}
	else break;
      }
    
    }


    // close connection
    close(client);
    close(s);
    return 0;
}
