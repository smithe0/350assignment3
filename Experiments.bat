 #!/bin/bash 
 COUNTER=0
 while [  $COUNTER -lt 100 ]; do
	./Testing
    let COUNTER=COUNTER+1 
 done