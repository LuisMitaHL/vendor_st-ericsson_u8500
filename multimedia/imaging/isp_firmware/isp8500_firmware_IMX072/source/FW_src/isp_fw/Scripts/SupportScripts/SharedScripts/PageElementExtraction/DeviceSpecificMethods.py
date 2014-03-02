		self.MailBoxIndexAddress        = 0x0064
		self.MailBoxOpcodeAddress       = 0x0068
		self.MailBoxWriteDataAddress    = 0x006c
		self.MailBoxReadDataAddress     = 0x0070
                self.HostComms_Operation_e_Read = 0
		self.HostComms_Operation_e_Write = 1
		self.HostComms_Status_ReadComplete = 1
		self.HostComms_Status_WriteComplete = 2
		self.HostComms_Status_ReadOnlyWriteDenied = 3

	def ReadIndiciesFromDevice(self, tSerialIndices):

		# Clear the Host Comms operation complete Event Notification
		self.commsTool.writeByte(0x0208, 0x02, self.CommsEcho)

		# compute the number of bytes to read
		bNumberOfBytes = len(tSerialIndices)

		# compute the base address from where the read will happen
		index = tSerialIndices[0]

		# write the value of index to PageElement register
		status = self.commsTool.writeByte(self.MailBoxIndexAddress, (index%0x100), self.CommsEcho)

		status = self.commsTool.writeByte((self.MailBoxIndexAddress + 1), index/0x100, self.CommsEcho)
		# read the value of transaction ID from the device
		LastTransactionId = self.commsTool.readByte((self.MailBoxOpcodeAddress + 1), self.CommsEcho)

		if (LastTransactionId >= 255):
		    LastTransactionId = 0

		# program the opcode to the OPCODE register
		self.commsTool.writeByte(self.MailBoxOpcodeAddress, self.HostComms_Operation_e_Read, self.CommsEcho)

		# program the transaction ID on the transaction ID register
		self.commsTool.writeByte((self.MailBoxOpcodeAddress + 1), LastTransactionId, self.CommsEcho)

		# Hack to trigger an interrupt into the ITM
		self.commsTool.writeByte(0x03D4, 0x01, self.CommsEcho)

		StatusComplete = 0
		# wait for the host comms event operation complete notification
		while ((StatusComplete & 0x02) == 0):
			StatusComplete = self.commsTool.readByte(0x0200, self.CommsEcho)

		codedValue = 0

		while (bNumberOfBytes != 0):
			bNumberOfBytes -= 1
			codedValue = (codedValue * 256) + self.commsTool.readByte(self.MailBoxReadDataAddress + bNumberOfBytes, self.CommsEcho)

		return codedValue
	
	def WriteIndiciesToDevice(self, tSerialIndices, codedValue):

		# Clear the Host Comms operation complete Event Notification
		self.commsTool.writeByte(0x0208, 0x02, self.CommsEcho)

		# compute the number of bytes to read
		bNumberOfBytes = len(tSerialIndices)

		# compute the base address from where the read will happen
		index = tSerialIndices[0]

		# write the value of index to PageElement register
		status = self.commsTool.writeByte(self.MailBoxIndexAddress, (index%0x100), self.CommsEcho)

		status = self.commsTool.writeByte((self.MailBoxIndexAddress + 1), index/0x100, self.CommsEcho)

        # read the value of transaction ID from the device
		LastTransactionId = self.commsTool.readByte((self.MailBoxOpcodeAddress + 1), self.CommsEcho)

		if (LastTransactionId >= 255):
			LastTransactionId = 0

		# write the value to be written on the host comms write data register
		for i in range (0, bNumberOfBytes):
			status = self.commsTool.writeByte((self.MailBoxWriteDataAddress + i), ((codedValue >> (i * 8)) & 0xFF), self.CommsEcho)

		# program the opcode to the OPCODE register
		self.commsTool.writeByte(self.MailBoxOpcodeAddress, self.HostComms_Operation_e_Write, self.CommsEcho)

		# program the transaction ID on the transaction ID register
		self.commsTool.writeByte((self.MailBoxOpcodeAddress + 1), LastTransactionId, self.CommsEcho)

		# Hack to trigger an interrupt into the ITM
		self.commsTool.writeByte(0x03D4, 0x01, self.CommsEcho)

		StatusComplete = 0
		# wait for the host comms event operation complete notification
		while ((StatusComplete & 0x02) == 0):
			StatusComplete = self.commsTool.readByte(0x0200, self.CommsEcho)

	def sVerifyDeviceType(self):

		# The purpose of this function is to verify that the current 
		# device wrapper is consistent with the device under test
		iActualDeviceId = self.ReadVariableValue('DeviceParameters.u32_DeviceId')
		iActualFirmwareVersionMajor = self.ReadVariableValue('DeviceParameters.u32_FirmwareVersionMajor')
		iActualFirmwareVersionMinor = self.ReadVariableValue('DeviceParameters.u32_FirmwareVersionMinor')
		iActualFirmwareVersionMicro = self.ReadVariableValue('DeviceParameters.u32_FirmwareVersionMicro')

		sActualDeviceVersion = str(iActualDeviceId) + ' v' + str(iActualFirmwareVersionMajor) + '.' + str(iActualFirmwareVersionMinor) + '.' + str(iActualFirmwareVersionMicro)
		sExpectedDeviceVersion = str(self.iExpectedDeviceId) + ' v' + str(self.FIRMWARE_VERSION_MAJOR) + '.' + str(self.FIRMWARE_VERSION_MINOR) + '.' + str(self.FIRMWARE_VERSION_MICRO)

		if sActualDeviceVersion != sExpectedDeviceVersion:
			message = "the 'device comms' object used appears to be "
			message += "inconsistent with the device under test "
			message += "(expected a device of type '%s'" % sExpectedDeviceVersion
			message += " vs. '%s') " % sActualDeviceVersion
			message += "[possible reasons for this include: use of the wrong "
			message += "device wrapper class, missing device under test, "
			message += "incorrect device under test version, not powering "
			message += "the device under test]"
			raise pst.FatalWrapperError(message)
		else:
			self.Log.info('Device under test verified to be ' + sActualDeviceVersion)
