const TOKEN = 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx';
const ATTR_HEADER_ROW = 1;
const FISRT_RECORD_ROW = 3;
const RECORDS_PERIOD = 24 * 60 * 60 * 1000; // 1 day
const CONTENTLENGTH_MAX = 1024;
const RECORDS_SHEET_NAME = 'Records';
const MIMETYPE_JSON = 'application/json';
const INVALID_DATE = 'Invalid Date'

function doGet(e) {
  return createResponse('Hello');
}

function doPost(e) {

  /*  Check parameters  */
  if (e === null || e.postData === null || e.postData.contents === null ||
      e.postData.type !== MIMETYPE_JSON || e.contentLength > CONTENTLENGTH_MAX) {
    return createResponse('Invalid Contents');
  }
  let requestObj = JSON.parse(e.postData.contents);
  if (requestObj === null || requestObj['token'] != TOKEN) {
    return createResponse('Mismatched Token');
  }
  let records = requestObj['records'];
  let numRecords = (records !== null) ? records.length : 0;
  if (numRecords == 0) {
    return createResponse('No records');
  }

  /*  Append new records  */
  let currentTime = Date.now();
  let ss = SpreadsheetApp.getActive()
  let sheet = ss.getSheetByName(RECORDS_SHEET_NAME);
  let lastDate = new Date(sheet.getRange(sheet.getLastRow(), 1).getValue());
  let recordTime = (lastDate.toString() === INVALID_DATE) ? currentTime : lastDate.getTime();
  let timeGap = (currentTime - recordTime) / numRecords;
  let headers = sheet.getRange(ATTR_HEADER_ROW, 1, 1, sheet.getLastColumn()).getValues()[0];

  records.map(function(record) {
    let values = [];
    recordTime += timeGap;
    headers.map(function(header) {
      let value = '';
      switch(header) {
        case 'date':
          value = new Date(recordTime);
          break;
        default:
          value = record[header];
          break;
      }
      values.push(value);
    });
    sheet.appendRow(values);
  });
  let lastRow = sheet.getLastRow();
  sheet.getRange(lastRow - numRecords + 1, 1, numRecords, 1).setNumberFormat('yy/M/d H:mm');

  /*  Delete old records  */
  let row = FISRT_RECORD_ROW;
  while (row <= lastRow) {
    let oldDate = new Date(sheet.getRange(row, 1).getValue());
    if (oldDate.toString() === INVALID_DATE || oldDate.getTime() > currentTime - RECORDS_PERIOD) {
      break;
    }
    row++;
  }
  if (row > FISRT_RECORD_ROW) {
    sheet.deleteRows(FISRT_RECORD_ROW, row - FISRT_RECORD_ROW);
  }

  return createResponse('Success');
}

function createResponse(msg) {
  let output = ContentService.createTextOutput(msg);
  output.setMimeType(ContentService.MimeType.TEXT);
  return output;
}

function doPostTest() {
  let records = [];
  for (let i = 0; i < 5; i++) {
    records.push({
      illum: 100 + Math.random() * 200,
      temp: 20 + Math.random() * 10
    });
  } 
  let requestObj = {
    token: TOKEN,
    records: records
  };
  let postData = new Object();
  postData.contents = JSON.stringify(requestObj);
  postData.type = MIMETYPE_JSON;
  let e = new Object();
  e.postData = postData;
  e.contentLength = postData.contents.length;
  doPost(e);
}
