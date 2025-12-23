#include "pch.h"
#include "Util.h"

vtkSmartPointer<vtkCellPicker> Util::CreateCellPicker()
{
	vtkSmartPointer<vtkCellPicker>  picker = vtkSmartPointer<vtkCellPicker>::New();

	return picker;
}

vtkSmartPointer<vtkPropPicker> Util::CreatePropPicker()
{
	vtkSmartPointer<vtkPropPicker>  picker = vtkSmartPointer<vtkPropPicker>::New();

	return picker;
}

vtkSmartPointer< vtkPolyData> Util::GeneratePolyDataNormal(vtkSmartPointer< vtkPolyData> polydata)
{
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalGenerator->SetInputData(polydata);
	normalGenerator->ComputePointNormalsOn();
	normalGenerator->ComputeCellNormalsOff();
	normalGenerator->Update();

	/*
	// Optional settings
	normalGenerator->SetFeatureAngle(0.1);
	normalGenerator->SetSplitting(1);
	normalGenerator->SetConsistency(0);
	normalGenerator->SetAutoOrientNormals(0);
	normalGenerator->SetComputePointNormals(1);
	normalGenerator->SetComputeCellNormals(0);
	normalGenerator->SetFlipNormals(0);
	normalGenerator->SetNonManifoldTraversal(1);
	*/

	return normalGenerator->GetOutput();
}

vtkSmartPointer<vtkPolyData> Util::CreatePolyData(vtkSmartPointer<vtkAppendPolyData> appender)
{
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->DeepCopy(appender->GetOutput());

	return GeneratePolyDataNormal(polydata);
}

vtkSmartPointer<vtkPolyData> Util::CreatePolyData(vtkSmartPointer<vtkSTLReader> reader)
{
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->DeepCopy(reader->GetOutput());

	return GeneratePolyDataNormal(polydata);
}


vtkSmartPointer<vtkPolyData> Util::CreatePolyData(vtkSmartPointer<vtkOBJReader> reader)
{
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->DeepCopy(reader->GetOutput());

	return GeneratePolyDataNormal(polydata);
}

vtkSmartPointer<vtkPolyData> Util::CreatePolyData(vtkSmartPointer<vtkPLYReader> reader)
{
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->DeepCopy(reader->GetOutput());

	return GeneratePolyDataNormal(polydata);
}


vtkSmartPointer<vtkPolyData> Util::CreatePolyData2(vtkSmartPointer<vtkSimplePointsReader> reader)
{
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->DeepCopy(reader->GetOutput());

	return GeneratePolyDataNormal(polydata);
}

vtkSmartPointer<vtkActor> Util::CreateActor(vtkSmartPointer<vtkPolyData> polyData)
{
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyData);
	mapper->SetScalarVisibility(true); // For distinguishing phantom and wearable color which are connected with one vtkPolyData

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	return actor;
}

vtkSmartPointer<vtkActor> Util::CreateDeepCopyActor(vtkSmartPointer<vtkPolyData> polyData)
{
	vtkSmartPointer<vtkPolyData> pPolyData = vtkSmartPointer<vtkPolyData>::New();
	pPolyData->DeepCopy(polyData);

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(pPolyData);

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	return actor;
}

vtkSmartPointer<vtkImageData> Util::CreateImageData(std::string image)
{
	QImage qimg(image.c_str());
	vtkSmartPointer<vtkQImageToImageSource> vtkQImage = vtkSmartPointer<vtkQImageToImageSource>::New();
	vtkQImage->SetQImage(&qimg);
	vtkQImage->Update();

	vtkSmartPointer<vtkImageData> imageData
		= vtkSmartPointer<vtkImageData>::New();
	imageData = vtkQImage->GetOutput();
	return imageData;
}

vtkSmartPointer<vtkActor> Util::CreateBoundingBox()
{
	double boxPoints[8][3] = { {-1, 1, 1}, {1, 1, 1}, {1, 1, -1}, {-1, 1, -1},
						{-1, -1, 1}, {1, -1, 1}, {1, -1, -1}, {-1, -1, -1} };

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < 8; ++i)
	{
		points->InsertNextPoint(boxPoints[i]);
	}

	vtkSmartPointer<vtkPolyLine> topline = vtkSmartPointer<vtkPolyLine>::New();
	topline->GetPointIds()->SetNumberOfIds(5);
	topline->GetPointIds()->SetId(0, 0);
	topline->GetPointIds()->SetId(1, 1);
	topline->GetPointIds()->SetId(2, 2);
	topline->GetPointIds()->SetId(3, 3);
	topline->GetPointIds()->SetId(4, 0);

	vtkSmartPointer<vtkPolyLine> bottomline = vtkSmartPointer<vtkPolyLine>::New();
	bottomline->GetPointIds()->SetNumberOfIds(5);
	bottomline->GetPointIds()->SetId(0, 4);
	bottomline->GetPointIds()->SetId(1, 5);
	bottomline->GetPointIds()->SetId(2, 6);
	bottomline->GetPointIds()->SetId(3, 7);
	bottomline->GetPointIds()->SetId(4, 4);

	vtkSmartPointer<vtkLine> line0 = vtkSmartPointer<vtkLine>::New();
	line0->GetPointIds()->SetId(0, 0);
	line0->GetPointIds()->SetId(1, 4);

	vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
	line1->GetPointIds()->SetId(0, 1);
	line1->GetPointIds()->SetId(1, 5);

	vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
	line2->GetPointIds()->SetId(0, 2);
	line2->GetPointIds()->SetId(1, 6);

	vtkSmartPointer<vtkLine> line3 = vtkSmartPointer<vtkLine>::New();
	line3->GetPointIds()->SetId(0, 3);
	line3->GetPointIds()->SetId(1, 7);

	// Create a cell array to store the lines in and add the lines to it
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(topline);
	cells->InsertNextCell(bottomline);
	cells->InsertNextCell(line0);
	cells->InsertNextCell(line1);
	cells->InsertNextCell(line2);
	cells->InsertNextCell(line3);

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(cells);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyData);

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(1, 0, 0);
	actor->GetProperty()->SetLineWidth(1.0);

	return actor;
}

vtkSmartPointer<vtkActor> Util::CopyVTKActor(vtkSmartPointer<vtkActor> spVTKActor, bool shareVTKPolyData)
{
	if (!spVTKActor) return nullptr;

	/// Original vtkPolyData: Share Only This Data
	vtkSmartPointer<vtkPolyData> spPolyData = nullptr;

	if (shareVTKPolyData) spPolyData = ((vtkPolyDataMapper*)(spVTKActor->GetMapper()))->GetInput();
	else				  spPolyData->DeepCopy(((vtkPolyDataMapper*)(spVTKActor->GetMapper()))->GetInput());

	if (!spPolyData) return nullptr;

	/// Create Independent vtkActor!
	vtkSmartPointer<vtkOpenGLPolyDataMapper> spNewVTKOpenGLPolyDataMapper = vtkSmartPointer<vtkOpenGLPolyDataMapper>::New();
	spNewVTKOpenGLPolyDataMapper->SetInputData(spPolyData);

	vtkSmartPointer<vtkActor> spNewPolyModelActor = vtkSmartPointer<vtkActor>::New();
	spNewPolyModelActor->SetMapper(spNewVTKOpenGLPolyDataMapper);

	/// Copy Material Property!
	vtkSmartPointer<vtkProperty> materialProperty_front = vtkSmartPointer<vtkProperty>::New();
	vtkSmartPointer<vtkProperty> materialProperty_back = vtkSmartPointer<vtkProperty>::New();

	materialProperty_front->DeepCopy(spVTKActor->GetProperty());
	materialProperty_back->DeepCopy(spVTKActor->GetBackfaceProperty());

	spNewPolyModelActor->SetProperty(materialProperty_front);
	spNewPolyModelActor->SetBackfaceProperty(materialProperty_back);

	/// Copy Actor's User Transform
	if (spVTKActor->GetUserMatrix()) spNewPolyModelActor->SetUserMatrix(spVTKActor->GetUserMatrix());

	/// Copy Actor's Internal Transform
	if (spVTKActor->GetOrigin())		spNewPolyModelActor->SetOrigin(spVTKActor->GetOrigin());
	if (spVTKActor->GetPosition())		spNewPolyModelActor->SetPosition(spVTKActor->GetPosition());
	if (spVTKActor->GetOrientation())	spNewPolyModelActor->SetOrientation(spVTKActor->GetOrientation());
	if (spVTKActor->GetScale())			spNewPolyModelActor->SetScale(spVTKActor->GetScale());

	return spNewPolyModelActor;
}


vtkSmartPointer<vtkActor> Util::CreateWireBoundingBox()
{
	double boxPoints[8][3] = { {-1, 1, 1}, {1, 1, 1}, {1, 1, -1}, {-1, 1, -1},
						{-1, -1, 1}, {1, -1, 1}, {1, -1, -1}, {-1, -1, -1} };

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < 8; ++i)
	{
		points->InsertNextPoint(boxPoints[i]);
	}

	vtkSmartPointer<vtkPolyLine> topline = vtkSmartPointer<vtkPolyLine>::New();
	topline->GetPointIds()->SetNumberOfIds(5);
	topline->GetPointIds()->SetId(0, 0);
	topline->GetPointIds()->SetId(1, 1);
	topline->GetPointIds()->SetId(2, 2);
	topline->GetPointIds()->SetId(3, 3);
	topline->GetPointIds()->SetId(4, 0);

	vtkSmartPointer<vtkPolyLine> bottomline = vtkSmartPointer<vtkPolyLine>::New();
	bottomline->GetPointIds()->SetNumberOfIds(5);
	bottomline->GetPointIds()->SetId(0, 4);
	bottomline->GetPointIds()->SetId(1, 5);
	bottomline->GetPointIds()->SetId(2, 6);
	bottomline->GetPointIds()->SetId(3, 7);
	bottomline->GetPointIds()->SetId(4, 4);

	vtkSmartPointer<vtkLine> line0 = vtkSmartPointer<vtkLine>::New();
	line0->GetPointIds()->SetId(0, 0);
	line0->GetPointIds()->SetId(1, 4);

	vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
	line1->GetPointIds()->SetId(0, 1);
	line1->GetPointIds()->SetId(1, 5);

	vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
	line2->GetPointIds()->SetId(0, 2);
	line2->GetPointIds()->SetId(1, 6);

	vtkSmartPointer<vtkLine> line3 = vtkSmartPointer<vtkLine>::New();
	line3->GetPointIds()->SetId(0, 3);
	line3->GetPointIds()->SetId(1, 7);

	// Create a cell array to store the lines in and add the lines to it
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(topline);
	cells->InsertNextCell(bottomline);
	cells->InsertNextCell(line0);
	cells->InsertNextCell(line1);
	cells->InsertNextCell(line2);
	cells->InsertNextCell(line3);

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(cells);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyData);

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(1, 0, 0);
	actor->GetProperty()->SetOpacity(0.4);
	actor->GetProperty()->SetLineWidth(1.0);
	actor->GetProperty()->SetRepresentationToWireframe();

	return actor;
}

vtkSmartPointer<vtkProperty> Util::GetProperty()
{
	double ambient[3] = { 1.0, 1.0, 1.0 };
	double diffusive[3] = { 1.0, 1.0, 1.0 };
	double specular[3] = { 1.0, 1.0, 1.0 };
	double reflection[4] = { 0.20, 0.6, 0.20, 100 };
	double opacity = 1.0;

	vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
	property->SetAmbient(reflection[0]);
	property->SetAmbientColor(ambient);
	property->SetDiffuse(reflection[1]);
	property->SetDiffuseColor(diffusive);
	property->SetSpecular(reflection[2]);
	property->SetSpecularPower(reflection[3]);
	property->SetSpecularColor(specular);
	property->SetOpacity(opacity);
	property->SetInterpolationToPhong();
	//property->SetInterpolationToFlat();

	return property;
}

namespace SkinDoseBinaryUtils
{
	void initializeBinaryFile(const QString& binaryFilePath)
	{
		QFile file(binaryFilePath);
		if (file.open(QIODevice::WriteOnly)) {
			QDataStream out(&file);
			out.setVersion(QDataStream::Qt_6_8);
			// 초기 헤더 공간 (0, 0.0) 확보
			out << static_cast<qint64>(0) << static_cast<double>(0.0);
			file.close();
		}
		else {
			qWarning() << "Failed to initialize binary file:" << binaryFilePath;
		}
	}

	bool appendFromTextFile(const QString& textFilePath, const QString& binaryFilePath)
	{
		// 1. 입력 텍스트 파일 열기
		QFile inputFile(textFilePath);
		if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qWarning() << "Cannot open source text file:" << textFilePath;
			return false;
		}
		QTextStream in(&inputFile);

		// 2. 텍스트 파일 헤더 파싱
		QString headerLine = in.readLine();
		//QStringList headerParts = headerLine.split(" ", Qt::SkipEmptyParts);
		QStringList headerParts = headerLine.split(" ", Qt::SkipEmptyParts);
		if (headerParts.size() < 2) {
			qWarning() << "Invalid header format in" << textFilePath;
			inputFile.close();
			return false;
		}
		qint64 nps = headerParts[0].toLongLong();
		double conversionFactor = headerParts[1].toDouble();

		// 3. 바이너리 파일 열기 (읽고 쓰기 모두 가능한 모드)
		QFile binaryFile(binaryFilePath);
		if (!binaryFile.open(QIODevice::ReadWrite)) {
			qWarning() << "Cannot open binary file for writing:" << binaryFilePath;
			inputFile.close();
			return false;
		}
		QDataStream stream(&binaryFile);
		stream.setVersion(QDataStream::Qt_6_8);

		// 4. 헤더 덮어쓰기
		binaryFile.seek(0);
		stream << nps << conversionFactor;

		// 5. 데이터 추가하기
		binaryFile.seek(binaryFile.size());

		while (!in.atEnd()) {
			QString line = in.readLine();
			//QStringList parts = line.split(" ", Qt::SkipEmptyParts);
			QStringList parts = line.split(" ", Qt::SkipEmptyParts);
			if (parts.size() < 5) continue;

			qint32 phantomId = parts[0].toInt();

			stream << phantomId << parts[1].toDouble() << parts[2].toDouble() << parts[3].toDouble() << parts[4].toDouble();
		}

		inputFile.close();
		binaryFile.close();
		return true;
	}

	std::pair<std::pair<qint64, double>, std::vector<SkinDoseData>> readAllData(const QString& binaryFilePath)
	{
		std::vector<SkinDoseData> allData;
		qint64 nps = 0;
		double conversionFactor = 0.0;

		QFile binaryFile(binaryFilePath);
		if (!binaryFile.open(QIODevice::ReadOnly)) {
			qWarning() << "Failed to open binary file for reading:" << binaryFilePath;
			return { {nps, conversionFactor}, allData };
		}

		QDataStream stream(&binaryFile);
		stream.setVersion(QDataStream::Qt_6_8);

		if (!stream.atEnd()) {
			stream >> nps >> conversionFactor;
		}

		while (!stream.atEnd()) {
			SkinDoseData data;
			stream >> data.phantomId >> data.x >> data.y >> data.z >> data.dE;
			allData.push_back(data);
		}

		binaryFile.close();
		qDebug() << "Read" << allData.size() << "records from binary file. NPS:" << nps;
		return { {nps, conversionFactor}, allData };
	}

} // namespace SkinDoseBinaryUtils