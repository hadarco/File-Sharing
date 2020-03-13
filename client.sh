#!/bin/bash

synopsis="NAME
    client - client side

SYNOPSIS
    api for communication with the file server.

    client -h|--help
      Show this synopsis.

    client [-s|--send] [-u|--user] user_name [-f|--file] file_name
      send a file to a specific user on the server

    client [-r|--receive] [-u|--user] user_name [-f|--file] file_name [-o|--output] path
      receive a file from a user and output to a specific path

    client [-a|--add] [-u|--user] user_name
      add a new user to the server"


user=""
file=""
out_path=""

flag_user="FALSE"
flag_file="FALSE"
flag_receive="FALSE"
flag_send="FALSE"
flag_add="FALSE"
flag_out="FALSE"

receive_file()
{

  if [ ! -e "$out_path" ]
    then
	   echo "Error invalid path."
	   return;
  fi
  error=""
  response=$(curl -o $out_path/$file localhost/download_file/$user/$file 2> err)

  if [ "$?" != "0" ]
    then
      error=$(cat err)
      echo "$error"
      rm -f err
      exit 1
  else
    echo "file received successfully"
  fi
}

send_file()
{
  if [ ! -e "$file" ]
    then
	   echo "Error file not exist."
	   return;
  fi
  
  if [ ! -f "$file" ]
    then
	   echo "Error not a file."
	   return;
  fi
  
  
  error=""
  response=$(curl --data-binary @$file localhost/upload_file/$user/$file 2> err)

  if [ "$?" != "0" ]
    then
      error=$(cat err)
      echo "$error"
      rm -f err
      exit 1
  else
    echo "file sent successfully"
  fi
}

add_user()
{
  error=""
  response=$(curl localhost/create_user/$user 2> err)

  if [ "$?" != "0" ]
    then
      error=$(cat err)
      echo "$error"
      rm -f err
      exit 1
  else
    echo "user added successfully"
  fi
}

# read the options
TEMP="getopt -o h::rsu:f:ao: --long help::,receive,send,user:,file:,addoutput: -n 'cf-dns' -- "$@" "
# echo "errorcode: $?"

output=$($TEMP 2> /tmp/error)
errorMessage=$(cat /tmp/error)

if [ "$?" != "0" -o "$errorMessage" != "" ]
  then
    echo $errorMessage
    rm -f /tmp/error
    exit 1
fi
# echo "temp: $TEMP"
eval set -- "$output"

while true ; do
    case "$1" in
        -h|--help)
            echo "$synopsis"
            exit 0
          ;;
        -r|--receive)
            flag_receive="TRUE"
            shift 1
          ;;
        -s|--send)
            flag_send="TRUE"
            shift 1
          ;;
        -u|--user)
            user=$2
            shift 2
            flag_user="TRUE"
          ;;
        -f|--file)
            file=$2
            shift 2
            flag_file="TRUE"
          ;;
        -a|--add)
            flag_add="TRUE"
            shift 1
          ;;
        -o|--output)
          out_path=$2
          shift 2
          flag_out="TRUE"
          ;;
        --)
            shift
            break
          ;;
        *)
            echo "Internal error!"
            exit 1
          ;;
    esac
done

if [ "$#" != "0" ]
  then
    echo "cf-dns: unknown extra arguments"
    exit 1
fi

if [ "$flag_receive" = "TRUE" -a "$flag_send" = "TRUE" ]
  then
    echo "cannot send and receive together."
    exit 1
fi

if [ "$flag_add" = "TRUE" -a "$flag_user" = "FALSE" ]
  then
    echo "cannot add, need to specify a user."
    exit 1
fi



if [ "$flag_receive" = "TRUE" -a "$flag_user" = "TRUE" -a "$flag_file" = "TRUE" ]
  then
    receive_file
fi

if [ "$flag_send" = "TRUE" -a "$flag_user" = "TRUE" -a "$flag_file" = "TRUE" ]
  then
    send_file
fi

if [ "$flag_add" = "TRUE" -a "$flag_user" = "TRUE" ]
  then
    add_user
fi
