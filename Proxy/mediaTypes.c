#include "include/mediaTypes.h"
#include "../Shared/include/lib.h"

int mediaRangeEvaluator(char **media_range_complete_list, char *media_type)
{
	int  ret                = NOT_INCLUDED_MEDIATYPE;
	char **split_media_type = divideMediaType(media_type);
	if(!isValidMediaType(split_media_type))
	{
		ret = INVALID_MEDIATYPE;
	}
	int  j = 0;
	char *media_range_complete;
	while((media_range_complete = media_range_complete_list[j]) && !ret)
	{
		char **split_media_range = divideMediaType(media_range_complete);
		if(mediaTypeBelongsToMediaRange(split_media_type, split_media_range))
		{
			ret = INCLUDED_MEDIATYPE;
		}
		j++;
		recursiveDoublePointerFree(split_media_range);
	}
	recursiveDoublePointerFree(split_media_type);

	return ret;
}