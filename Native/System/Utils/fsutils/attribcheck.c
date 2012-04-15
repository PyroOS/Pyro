#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pyro/fs_attribs.h>

int main(int argc, char *argv[])
{
	int nRetVal = 1;

	if (argc < 3)
		printf("usage: %s path attr_name\n", argv[0]);

	else
	{
		register char *pBuffer;
		register int hFd, hFdCopy;
		struct attr_info sAttrInfo;

		if ((hFd = open(argv[1], O_RDONLY | O_NOTRAVERSE)) >= 0)
		{
			if (stat_attr( hFd, argv[2], &sAttrInfo) == 0)
			{
				if ((pBuffer = (char *)malloc(sAttrInfo.ai_size)) != NULL)
				{
					if (read_attr(hFd, argv[2], sAttrInfo.ai_type, pBuffer, 0, sAttrInfo.ai_size) == sAttrInfo.ai_size)
					{
						if (printf("%s", pBuffer))
							nRetVal = 0;
						else
							fprintf(stderr, "Failed to dump attrib %s\n", argv[2]);
					}

					else
						fprintf(stderr, "Failed to read attrib %s\n", argv[2]);

					free(pBuffer);
				}

				else
					fprintf(stderr, "Out of memory!\n");
			}

			close(hFdCopy);
		}

		else
			fprintf(stderr, "Failed to open %s: %s\n", argv[1], strerror(errno));
	}

	return nRetVal;
}
