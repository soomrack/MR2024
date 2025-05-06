from typing import Any, Iterator

import numpy as np
from geopandas import GeoDataFrame, GeoSeries
from pyproj import CRS
from rasterio import DatasetReader, open as open_raster, transform as raster_transform
from rasterio.mask import mask as mask_band

from .constants import DRIVERS
from .metadata import Metadata
from .utils import reproject_dataset, to_dataset
from ..shapes.polygon import PolygonShapefile


class Layer:
    """
    Represents a single layer of raster data.
    """

    def __init__(
            self,
            data: DatasetReader | np.ndarray | None = None,
            metadata: Metadata | dict | None = None
    ) -> None:
        """
        Initializes a new Layer instance.

        Args:
            data (DatasetReader | np.ndarray | None): The raster data.
            metadata (Metadata | dict | None): Metadata associated with the layer.
        """
        self._metadata = metadata

        # Convert metadata to Metadata object if it's a dictionary
        if isinstance(self._metadata, dict):
            self._metadata = Metadata(**self._metadata)

        # Convert data to a DatasetReader and validate it
        self._data = to_dataset(data, self._metadata.to_dict())
        self._validate_data()

    def _validate_data(self) -> None:
        """
        Validates the data in the layer.
        This method can be overridden by subclasses to add specific validation logic.
        """
        pass

    @staticmethod
    def _unpack(stack: np.ndarray) -> np.ndarray:
        """
        Unpacks a single-layer stack into a 2D array if necessary.

        Args:
            stack (np.ndarray): The input array.

        Returns:
            np.ndarray: The unpacked array.
        """
        if stack.shape[0] == 1 and len(stack.shape) == 3:
            return stack[0]
        return stack

    @property
    def crs(self) -> CRS | str:
        """
        Gets the Coordinate Reference System (CRS) of the layer.

        Returns:
            CRS | str: The CRS of the layer.
        """
        return self._metadata.crs

    @crs.setter
    def crs(self, crs: CRS | str) -> None:
        """
        Sets the Coordinate Reference System (CRS) of the layer.

        Args:
            crs (CRS | str): The CRS to set.
        """
        if isinstance(crs, str):
            crs = CRS.from_user_input(crs)
        elif not isinstance(crs, CRS):
            raise ValueError("Invalid CRS type.")

        # Reproject the dataset and update metadata
        self._data = reproject_dataset(self._data, crs)
        self._metadata.update(metadata=self._data.meta)

    def crop(self, shapefile: PolygonShapefile | GeoDataFrame | GeoSeries, **kwargs) -> "Layer":
        """
        Crops the layer using a given shapefile or GeoDataFrame.

        Args:
            shapefile (PolygonShapefile | GeoDataFrame | GeoSeries): The shapefile or GeoDataFrame to use for cropping.
            **kwargs: Additional keyword arguments for cropping.

        Returns:
            Layer: The cropped layer.
        """
        if self._data is None:
            raise ValueError("Layer is empty.")

        # Handle specific geometry number if provided
        features = kwargs.pop("features", None)

        if isinstance(features, list) and isinstance(shapefile, PolygonShapefile):
            shapefile = [shapefile.geometry_feature(number) for number in features]

        elif isinstance(features, int) and isinstance(shapefile, PolygonShapefile):
            shapefile = shapefile.geometry_feature(features)

        elif isinstance(shapefile, PolygonShapefile):
            shapefile = shapefile.geometry

        # Ensure shapefile is a list of geometries
        if isinstance(shapefile, (GeoDataFrame, GeoSeries)):
            shapefile = [shapefile]

        # Crop the data using the provided shapefile
        for geometry in shapefile:
            data, transform = mask_band(self.data, geometry, crop=True, **kwargs)
            self._metadata.update_after_crop(transform=transform, crop=data[0])
            self._data = to_dataset(data, self._metadata.to_dict())

        return self

    @classmethod
    def from_layer(cls, *args, **kwargs) -> "Layer":
        """
        Creates a new Layer instance from an existing layer.

        Args:
            *args: Positional arguments.
            **kwargs: Keyword arguments.

        Returns:
            Layer: A new Layer instance.

        Raises:
            NotImplementedError: This method should be implemented by subclasses.
        """
        raise NotImplementedError

    def get_by_index(self, row: int, col: int):
        """
        Gets the value at a specific row and column index in the raster data.

        Args:
            row (int): The row index.
            col (int): The column index.

        Returns:
            The value at the specified index.
        """
        array = self.to_numpy()

        if row >= self.shape[0] or col >= self.shape[1]:
            raise ValueError("Invalid row or column index.")

        return array[row][col]

    @property
    def data(self) -> DatasetReader:
        """
        Gets the raster data.

        Returns:
            DatasetReader: The raster data.
        """
        return self._data

    @property
    def max(self) -> int | float:
        """
        Gets the maximum value in the raster data.

        Returns:
            int | float: The maximum value.
        """
        return self.to_numpy().max()

    @property
    def mean(self) -> int | float:
        """
        Gets the mean value in the raster data.

        Returns:
            int | float: The mean value.
        """
        return self._data.read().mean()

    @property
    def metadata(self) -> Metadata:
        """
        Gets the metadata associated with the layer.

        Returns:
            Metadata: The metadata.
        """
        return self._metadata

    @metadata.setter
    def metadata(self, metadata: Metadata) -> None:
        """
        Sets the metadata associated with the layer.

        Args:
            metadata (Metadata): The metadata to set.
        """
        self._metadata = metadata

    @property
    def min(self) -> int | float:
        """
        Gets the minimum value in the raster data.

        Returns:
            int | float: The minimum value.
        """
        return self._data.read().min()

    @property
    def shape(self) -> tuple[int, ...]:
        """
        Gets the shape of the raster data.

        Returns:
            tuple: The shape of the raster data.
        """
        return self._data.shape

    def to_numpy(self) -> np.ndarray:
        """
        Converts the raster data to a NumPy array.

        Returns:
            np.ndarray: The raster data as a NumPy array.
        """
        return self._unpack(self._data.read().astype(np.float32))


class Raster:
    """
    Represents a raster with multiple layers.
    """

    _data: DatasetReader
    _metadata: Metadata
    _bands: list[Layer]

    def __init__(
            self,
            data: DatasetReader | None = None,
            bands: list[Layer] | None = None,
            metadata: Metadata | dict | None = None,
            **kwargs
    ) -> None:
        """
        Initializes a new Raster instance.

        Either `data` or `bands` must be provided. If `data` is provided, it initializes the raster
        using the dataset reader and extracts layers from it. If `bands` is provided, it initializes
        the raster using the list of layers and constructs the dataset from these layers.

        Args:
            data (DatasetReader | None): The raster data. If provided, `bands` should be None.
            bands (list[Layer] | None): The layers of the raster. If provided, `data` should be None.
            metadata (Metadata | dict | None): Metadata associated with the raster. If provided as a dictionary,
                                               it will be converted into a `Metadata` object.

        Raises:
            ValueError: If neither `data` nor `bands` is provided.
        """
        if (data is None and bands is None) or (data is not None and bands is not None):
            raise ValueError("Either data or bands must be provided.")

        self._metadata = metadata

        # Convert metadata to Metadata object if it's a dictionary
        if isinstance(self._metadata, dict):
            self._metadata = Metadata(**self._metadata)

        if data is not None:
            self._data = data
            self._bands = self._extract_layers()

            if self._metadata is None:
                self._metadata = Metadata(**self._data.meta)

        else:
            self._bands = bands  # noqa
            self._data = to_dataset(np.dstack([layer.to_numpy() for layer in bands]), self._metadata.to_dict())  # noqa

        if crs := kwargs.pop("crs", None):
            self.crs = crs

    def __getitem__(self, item: int) -> Layer:
        """
        Gets a specific layer from the raster.

        Args:
            item (int): The index of the layer to get.

        Returns:
            Layer: The specified layer.
        """
        return self._bands[item]

    def __setitem__(self, key: int, value: Layer) -> None:
        """
        Sets a specific layer in the raster.

        Args:
            key (int): The index of the layer to set.
            value (Layer): The layer to set.
        """
        self._bands[key] = value

    def __iter__(self) -> Iterator[Layer]:
        """
        Iterates over the layers in the raster.

        Returns:
            iterator: An iterator over the layers.
        """
        return iter(self._bands)

    def __len__(self) -> int:
        """
        Gets the number of layers in the raster.

        Returns:
            int: The number of layers.
        """
        return len(self._bands)

    def _extract_layers(self) -> list[Layer]:
        """
        Extracts individual layers from the raster data.

        Returns:
            list[Layer]: A list of layers.
        """
        return [
            Layer(data=self._data.read(i + 1).astype(np.float32), metadata=self._metadata) for i in range(self._data.count)
        ]

    def _get_bands_to_save(self, bands_nums: list[int] | int | None = None) -> np.ndarray:
        """
        Gets the bands to save.

        Args:
            bands_nums (list[int] | int | None): The indices of the bands to save. If None, all bands are saved.

        Returns:
            np.ndarray: The bands to save as a NumPy array.
        """
        if bands_nums is None:
            bands_nums = list(range(self._data.count))
        elif isinstance(bands_nums, int):
            bands_nums = [bands_nums]

        return np.array([self[band].to_numpy() for band in bands_nums])

    @staticmethod
    def _unpack(stack: np.ndarray) -> np.ndarray:
        """
        Unpacks a single-layer stack into a 2D array if necessary.

        Args:
            stack (np.ndarray): The input array.

        Returns:
            np.ndarray: The unpacked array.
        """
        if stack.shape[0] == 1 and len(stack.shape) == 3:
            return stack[0]
        return stack


    def coordinates(self) -> tuple[np.ndarray, np.ndarray]:
        """
        Generates the latitude and longitude coordinates for each pixel in the raster.

        Returns:
            tuple: Two arrays representing the latitude and longitude coordinates.
        """
        cols, rows = np.meshgrid(
            np.arange(self._data.width),
            np.arange(self._data.height),
            indexing='ij'
        )

        longitudes, latitudes = raster_transform.xy(self._data.transform, rows, cols)  # noqa

        return np.array(latitudes), np.array(longitudes)

    def bands(self, merge: bool = False) -> np.ndarray:
        """
        Retrieves the bands of the raster as numpy array.

        Args:
            merge (bool): Whether to merge the bands into a single array.

        Returns:
            np.ndarray: The bands as a NumPy array.
        """
        bands = [layer.to_numpy() for layer in self.layers]

        if merge:
            bands = np.stack(bands, axis=0)

        return self._unpack(bands[0] if isinstance(bands, list) else bands)


    @property
    def crs(self) -> CRS | str:
        """
        Gets the Coordinate Reference System (CRS) of the raster.

        Returns:
            CRS: The CRS of the raster.
        """
        return self.metadata.crs

    @crs.setter
    def crs(self, crs: CRS | str) -> None:
        """
        Sets the Coordinate Reference System (CRS) of the raster.

        Args:
            crs (CRS | str): The CRS to set.
        """
        if isinstance(crs, str):
            crs = CRS.from_user_input(crs)
        elif not isinstance(crs, CRS):
            raise ValueError("CRS must be a string or a `CRS` object")

        # Reproject the dataset and update metadata
        self._data = reproject_dataset(self._data, crs)
        self._metadata.update(metadata=self._data.meta)

        # Update CRS for each band
        for band in self._bands:
            band.crs = crs

    def crop(
            self,
            shapefile: PolygonShapefile | GeoDataFrame | GeoSeries,
            bands: list[int] | int | None = None,
            **kwargs
    ) -> "Raster":
        """
        Crops the raster using a given shapefile or GeoDataFrame.

        Args:
            shapefile (PolygonShapefile | GeoDataFrame | GeoSeries): The shapefile or GeoDataFrame to use for cropping.
            bands (list[int] | int | None): The indices of the bands to crop. If None, all bands are cropped.
            **kwargs: Additional keyword arguments for cropping.

        Returns:
            Raster: The cropped raster.
        """
        if isinstance(bands, int):
            bands = [bands]

        elif bands is None:
            bands = list(range(self._data.count))

        # Crop each specified band
        for band in bands:
            self[band] = self[band].crop(shapefile, **kwargs)
            self._metadata.update(metadata=self[band].metadata)

        return self

    @classmethod
    def from_file(cls, filepath: str, **kwargs) -> "Raster":
        """
        Creates a new Raster instance from a file.

        Args:
            filepath (str): The path to the file.
            **kwargs: Additional keyword arguments for opening the file.

        Returns:
            Raster: A new Raster instance.
        """
        options = kwargs.pop("options", {})

        # Determine the appropriate driver based on the file extension
        driver = options.pop("driver", None)
        if not driver:
            driver = DRIVERS.get(filepath.split(".")[-1], None)

        data = open_raster(filepath, driver=driver, **options)
        return cls(data=data, metadata=data.meta, **kwargs)

    @classmethod
    def from_raster(cls, *args, **kwargs) -> "Raster":
        """
        Creates a new Raster instance from an existing raster.

        Args:
            *args: Positional arguments.
            **kwargs: Keyword arguments.

        Returns:
            Raster: A new Raster instance.

        Raises:
            NotImplementedError: This method should be implemented by subclasses.
        """
        raise NotImplementedError

    def intersect_raster(self, other: "Raster") -> None:
        """
        Intersects this raster with another raster.

        Args:
            other (Raster): The other raster to intersect with.

        Raises:
            NotImplementedError: This method is not yet implemented.
        """
        raise NotImplementedError

    @property
    def layers(self) -> list[Layer]:
        """
        Gets the layers in the raster.

        Returns:
            list[Layer]: The layers in the raster.
        """
        return self._bands

    def max(self, band: int = 0) -> int | float:
        """
        Returns the maximum value in the specified band of the dataset.

        Args:
            band (int): The index of the band from which to retrieve the maximum value. Default is 0.

        Returns:
            int | float: The maximum value in the specified band of the dataset.
        """
        return self[band].max

    def mean(self, band: int = 0) -> int | float:
        """
        Compute the mean value for a specified band of data.

        Args:
            band (int): The band for which the mean is to be computed. Defaults to 0.

        Returns:
            int | float: The calculated mean value of the data in the specified band.
        """
        return self[band].mean

    @property
    def metadata(self) -> Metadata:
        """
        Gets the metadata associated with the raster.

        Returns:
            Metadata: The metadata.
        """
        return self._metadata

    def min(self, band: int = 0) -> int | float:
        """
        Computes the minimum value of the specified band in a data structure that contains multi-band data.

        Parameters:
            band (int): The index of the band for which the minimum value is to be computed. Defaults to 0.

        Returns:
            int | float: The minimum value found in the specified band of the data structure.
        """
        return self[band].min

    def nearest_pixel(self, latitude: float, longitude: float) -> tuple[int, tuple[float, float]]:
        """
        Finds the nearest pixel to the given longitude and latitude.

        Args:
            latitude (float): The latitude coordinate.
            longitude (float): The longitude coordinate.

        Returns:
            tuple: A tuple containing the index and coordinates of the nearest pixel.
        """
        latitudes, longitudes = self.coordinates()

        lat_diff = np.abs(latitudes - latitude)
        lon_diff = np.abs(longitudes - longitude)

        combined_diff = np.sqrt(lat_diff ** 2 + lon_diff ** 2)

        idx = np.argmin(combined_diff)

        return idx, (latitudes[idx], longitudes[idx])  # noqa


    def pixel_info(self, latitude: float, longitude: float, band_num: int = 0) -> dict[str, Any]:
        """
        Gets information about the pixel closest to the given longitude and latitude.

        Args:
            latitude (float): The latitude coordinate.
            longitude (float): The longitude coordinate.
            band_num (int): The band number to get information from. Default is 0.

        Returns:
            dict: A dictionary containing the band number, latitude, longitude, and value of the pixel.
        """
        index, coords = self.nearest_pixel(latitude, longitude)
        value = self[band_num].get_by_index(index // self.shape[-1], index % self.shape[-1])

        return {
            "band": band_num,
            "latitude": coords[1],
            "longitude": coords[0],
            "value": value
        }

    @property
    def shape(self) -> tuple[int, ...]:
        """
        Gets the shape of the raster data.

        Returns:
            tuple: The shape of the raster data.
        """
        return self._data.shape

    def to_file(
            self,
            filepath: str,
            bands_to_save: list[int] | int | None = None,
            **kwargs
    ) -> None:
        """
        Saves the raster to a file.

        Args:
            filepath (str): The path to the file.
            bands_to_save (list[int] | int | None): The indices of the bands to save. If None, all bands are saved.
            **kwargs: Additional keyword arguments for saving the file.
        """
        bands = self._get_bands_to_save(bands_to_save)

        with open_raster(fp=filepath, mode='w', **self._metadata.to_dict(), **kwargs) as file:
            try:
                file.write(bands)
            except ValueError:
                file.write(bands[0])
