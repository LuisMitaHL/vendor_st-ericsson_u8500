BEGIN { allflags="" }
{
  #check for and go to next if it is a comment
  gsub(/^#.*/,"",$0);
  if($0=="") next

  #replace * with .*
  gsub(/\./,"\\.",$0);
  gsub(/\*/,".*",$0);
  #printf "Name: %s", $0

  #split in directory, filename and flag fields
  split($0, arr, ":");
  directory=arr[1];
  flags=arr[2];
  amount = split(directory, arr, "/");
  if (amount == 0) {
    filename = directory;
    directory = "";
  } else {
    filename=arr[amount];
    sub(/[^\/]+$/, "", directory);
  }

  #if one of directory or filename is emtpy but not the other the other should match everything
  if(directory=="" && filename!="") directory=".*"
  if(filename=="" && directory!="") filename=".*"
  #printf "\n\tdirectory %s\n\tfilename %s\n\tflags %s\n", directory, filename, flags

  #split path in "input" to directory and filename
  indirectory=input
  amount = split(indirectory, arr, "/");
  if (amount == 0) {
    infilename = indirectory;
    indirectory = "";
  } else {
    infilename=arr[amount];
    sub(/[^\/]+$/, "", indirectory);
  }
  #printf "\n\tdirectory %s\n\tfilename %s\n", indirectory, infilename

  #compare input with the content of the record in the file
  directorymatch=0
  filenamematch=0;
  if(directory!="" && filename!="")
  {
    directorymatch=match(indirectory,directory);
    filenamematch=match(infilename,filename);
  }
  #printf "directorymatch %d, filenamematch %d\n", directorymatch, filenamematch

  #...and if they match return the flag
  if(directorymatch==1 && filenamematch==1)
  {
    #gsub(/[[:space:]]*/,"",flags)
    #printf("#%s#\n",flags);
    allflags=sprintf("%s %s", allflags, flags);
    #printf("%s\n",allflags);
    #exit 0
  }
}
END { printf("%s",allflags); }
