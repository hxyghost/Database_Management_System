#include "pfm.h"

PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance() {
	if (!_pf_manager)
		_pf_manager = new PagedFileManager();

	return _pf_manager;
}

PagedFileManager::PagedFileManager() {

}

PagedFileManager::~PagedFileManager() {
}

/*
 * return 0: success, return 1: file already exists, return -1: file not exists and create failed
 */
RC PagedFileManager::createFile(const string &fileName) {
	if (access(fileName.c_str(), F_OK) != 0) {
		cout << "file not exists" << endl;
		FILE *f = fopen(fileName.c_str(), "w+");
		if (f != NULL) {
			fclose(f);
			return 0;
		} else {
			return -1;
		}

	} else {
		cout << "file already exists" << endl;
		return 1;
	}

}

RC PagedFileManager::destroyFile(const string &fileName) {

	int result = remove(fileName.c_str());
	if (result == 0) {
		return 0;
	} else {
		cout << "destroy file failed " << endl;
		return -1;
	}

}

/*
 * success : 0, fail: -1
 */
RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle) {
	if (access(fileName.c_str(), F_OK) != 0) {
		cout << "file not exists " << endl;
		return -1;

	} else {
		if(fileHandle.setPointer(fileName)==0){
			return 0;
		}else{
			return -1;
		}
	}


}

RC PagedFileManager::closeFile(FileHandle &fileHandle) {
	return fileHandle.closePointer();
}

FileHandle::FileHandle() {
	readPageCounter = 0;
	writePageCounter = 0;
	appendPageCounter = 0;
	pointer = NULL;
	num_of_pages=0;
}

FileHandle::~FileHandle() {
	closePointer();
}

RC FileHandle::readPage(PageNum pageNum, void *data) {
	if(pageNum > num_of_pages-1){
		return -1;
	}
	fseek(pointer,pageNum*PAGE_SIZE,SEEK_SET);
	if(fread(data,PAGE_SIZE,1,pointer)!=1){
		return -1;
	}else{
		readPageCounter++;
		return 0;
	}
}

RC FileHandle::writePage(PageNum pageNum, const void *data) {
	if(pageNum > num_of_pages -1){
		return -1;
	}
	fseek(pointer,pageNum*PAGE_SIZE,SEEK_SET);
	if(fwrite(data,PAGE_SIZE,1,pointer)!=1){
		return -1;
	}else{
		writePageCounter++;
		return 0;
	}
}

RC FileHandle::appendPage(const void *data) {
	fseek(pointer,0,SEEK_END);
	if(fwrite(data,PAGE_SIZE,1,pointer)!=1){
		return -1;
	} else {
		appendPageCounter++;
		num_of_pages++;
		return 0;
	}

}

unsigned FileHandle::getNumberOfPages() {
	return num_of_pages;
}

RC FileHandle::collectCounterValues(unsigned &readPageCount,
		unsigned &writePageCount, unsigned &appendPageCount) {
	readPageCount = readPageCounter;
	writePageCount = writePageCounter;
	appendPageCount = appendPageCounter;
	return 0;
}


RC FileHandle::setPointer(const string &fileName){
	pointer= fopen(fileName.c_str(), "r+");
	if(pointer == NULL){
		return -1;
	}else{
		fseek(pointer,0,SEEK_END);
		num_of_pages = ceil(ftell(pointer)/PAGE_SIZE);
		return 0;
	}
}

RC FileHandle::closePointer(){
	if(fclose(pointer)==0){
		return 0;
	}else{
		return -1;
	}
}
