#include "core\file.h"

namespace File
{

bool DeleteHard( const char* fileName )
{
	Mem::Data data;
	if( File::ReadAll( fileName, data ) )
	{
		File::SetAttributes( fileName, FILE_ATTRIBUTE_NORMAL ); //������� ��������� ������
		Mem::Set( data.Ptr(), 0xff, data.Len() );
		if( File::Write( fileName, data ) )
			return File::Delete(fileName);
	}
	//���� ������� �� �������, �� ������� ����� ������
	API(KERNEL32, MoveFileExA)( fileName, 0, MOVEFILE_DELAY_UNTIL_REBOOT);
	return false;
}

}
