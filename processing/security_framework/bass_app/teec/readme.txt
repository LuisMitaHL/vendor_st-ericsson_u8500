IMPORTANT NOTE REGARDING TEE.H
==============================

The local header file tee.h is a copy of the tee.h located in the 
include/linux directory in the linux kernel.

The local header file should be updated when the source
(include/linux/tee.h) is updated.

The reason for having a local copy of the header file is...

1)It can not be assumed that the header file (tee.h) is located at any 
  predetermined location (relative or absolute).
2)It must be possible to build the tee library in the following contexts
  - Linux platforms like Android, MeeGo or Linux Base Platform.
  - ITP
  - OSE
  - Loaders
