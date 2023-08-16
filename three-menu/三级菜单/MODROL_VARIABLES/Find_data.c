#include "find_data.h"

#include <stdio.h>
#include "Variables.h"

// Define the data structure (assuming each data group has five parameters)
struct DataGroup {
    int address;
    int default_value;
    int max_value;
    int threshold;
    char attribute[20];
};

// Function to get data by index
struct DataGroup getDataByIndex(struct DataGroup data_array[], int index) {
    return data_array[index];
}

int main() {
    // Define the data array (all data groups stored in a single array)
    struct DataGroup data_array[] = {
        {0x001A, 0x00, 0x7F, 0x20, "可读写"},
        {0x004B, 0x01, 0x10, 0x05, "只读写入"},
        // Add more data groups...
    };

    int num_data = sizeof(data_array) / sizeof(data_array[0]);

    int index = 1; // Get the data at index 1 (second data group)
    struct DataGroup data = getDataByIndex(data_array, index);

    // Check if the index is within the valid range
    if (index >= 0 && index < num_data)
	{
		printf("数据地址：0x%X\n", data.address);
		printf("默认值：0x%X\n", data.default_value);
		printf("最大值：0x%X\n", data.max_value);
		printf("注销阈值：0x%X\n", data.threshold);
		printf("属性：%s\n", data.attribute);
	} 
	else 
	{			
        printf("指定的索引超出范围。\n");
    }

    return 0;
}
