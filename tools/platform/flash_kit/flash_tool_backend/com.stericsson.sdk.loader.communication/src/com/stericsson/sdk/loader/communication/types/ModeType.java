package com.stericsson.sdk.loader.communication.types;

import java.io.Serializable;

/**
 * @author xdancho
 * 
 */
public class ModeType implements Serializable {

    private static final long serialVersionUID = 8337142132329978313L;

    static final int FILE_TYPE_MASK = 0xF000; // 0170000;

    static final int ACCESS_OWNER_MASK = 0x01C0;

    static final int ACCESS_GROUP_MASK = 0x0038;

    static final int ACCESS_OTHERS_MASK = 0x0007;

    static final int UNIX_DOMAIN_SOCKET = 0xC000; // 0140000;

    static final int SYMBOLIC_LINK = 0xA000; // 0120000;

    static final int REGULAR_FILE = 0x8000; // 0100000;

    static final int BLOCK_DEVICE = 0x6000; // 0060000;

    static final int DIRECTORY_FILE = 0x4000; // 0040000;

    static final int CHARACTER_DEVICE = 0x2000; // 0020000;

    static final int NAMED_PIPE = 0x1000; // 0010000;

    static final int PERMISSION_STRING_SIZE = 3;

    /** */
    public static final String REGULAR_FILE_STR = "File";
    /** */
    public static final String UNIX_DOMAIN_SOCKET_STR = "Unix domain socket";
    /** */
    public static final String SYMBOLIC_LINK_STR = "Symbolic link";
    /** */
    public static final String BLOCK_DEVICE_STR = "Block device";
    /** */
    public static final String DIRECTORY_FILE_STR = "Directory";
    /** */
    public static final String CHARACTER_DEVICE_STR = "Character device";
    /** */
    public static final String NAMED_PIPE_STR = "Named pipe";

    int accessPermissionOwner;

    int accessPermissionGroup;

    int accessPermissionOthers;

    String type;

    int modeValue;

    /**
     * @param mode
     *            mode value
     * @throws Exception
     *             If file type could not be determined
     */
    public ModeType(int mode) throws Exception {

        modeValue = mode;

        switch (mode & FILE_TYPE_MASK) {

        case UNIX_DOMAIN_SOCKET:
            type = UNIX_DOMAIN_SOCKET_STR;
            break;

        case SYMBOLIC_LINK:
            type = SYMBOLIC_LINK_STR;
            break;

        case REGULAR_FILE:
            type = REGULAR_FILE_STR;
            break;

        case BLOCK_DEVICE:
            type = BLOCK_DEVICE_STR;
            break;

        case DIRECTORY_FILE:
            type = DIRECTORY_FILE_STR;
            break;

        case CHARACTER_DEVICE:
            type = CHARACTER_DEVICE_STR;
            break;

        case NAMED_PIPE:
            type = NAMED_PIPE_STR;
            break;

        default:
            throw new Exception("Invalid file type");

        }

        if (mode != 0) {
            accessPermissionOwner = (mode & ACCESS_OWNER_MASK) >> 6;
            accessPermissionGroup = (mode & ACCESS_GROUP_MASK) >> 3;
            accessPermissionOthers = mode & ACCESS_OTHERS_MASK;
        }
    }

    /**
     * @return The access permissions for the owner
     */
    public int getAccessPermissionOwner() {
        return accessPermissionOwner;
    }

    /**
     * @return The access permissions for the group
     */
    public int getAccessPermissionGroup() {
        return accessPermissionGroup;
    }

    /**
     * @return The access permissions for others
     */
    public int getAccessPermissionOthers() {
        return accessPermissionOthers;
    }

    /**
     * @return Type of file
     */
    public String getType() {
        return type;
    }

    /**
     * @return The file mode flags value
     */
    public int getModeValue() {
        return modeValue;
    }
}
